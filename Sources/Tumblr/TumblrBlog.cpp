#include "TumblrPCH.h"
#include "TumblrBlog.h"
#include "Utils/HttpRequest.h"
#include "Utils/Image.h"


// The max number of consecutive medias we already have
#define MAX_CONSECUTIVE_MEDIAS 20


namespace Tumblr
{

	//!
	//! Default constructor
	//!
	Blog::Blog(void)
		: Blog("", "")
	{
	}

	//!
	//! Construct from a Url
	//!
	Blog::Blog(const QString & name)
		: Blog(name, "")
	{
	}

	//!
	//! Full constructor
	//!
	Blog::Blog(const QString & name, const QString & outputFolder)
		: QObject(nullptr)
		, m_Name(name)
		, m_OutputFolder(outputFolder)
		, m_MaxPageUpdate(10)
		, m_TaskManager(1)
		, m_NetworkManager(nullptr)
		, m_CancelUpdate(false)
		, m_Consecutive(0)
	{
	}

	//!
	//! Copy constructor
	//!
	Blog::Blog(const Blog & other)
		: Blog(other.m_Name, other.m_OutputFolder)
	{
		m_Medias = other.m_Medias;
	}

	//!
	//! Destructor
	//!
	Blog::~Blog(void)
	{
		// dirty hack : wait for the updates to finish
		QMutexLocker lock(&m_UpdateMutex);
	}

	//!
	//! Serialize to a QJsonObject
	//!
	void Blog::Serialize(QJsonObject & desc) const
	{
		// serialize the medias
		QJsonArray medias;
		for (const QString & media : m_Medias)
		{
			medias.append(media);
		}

		// and the todos
		QJsonArray todos;
		for (const QString & todo : m_Todos)
		{
			todos.append(todo);
		}

		// serialize the blog
		desc["name"]	= m_Name;
		desc["output"]	= m_OutputFolder;
		desc["maxPage"]	= m_MaxPageUpdate;
		desc["avatar"]	= ImageToString(m_Avatar);
		desc["medias"]	= medias;
		desc["todos"]	= todos;
	}

	//!
	//! Deserialize from a QJsonObject
	//!
	void Blog::Deserialize(const QJsonObject & desc)
	{
		// general infos
		m_Name			= desc["name"].toString(),
		m_OutputFolder	= desc["output"].toString();
		m_MaxPageUpdate	= desc.contains("maxPage") == true ? desc["maxPage"].toInt() : 10;
		m_Avatar		= StringToImage(desc["avatar"].toString());

		// load medias and todos
		for (QJsonValueRef media : desc["medias"].toArray())
		{
			m_Medias.insert(media.toString());
		}
		for (QJsonValueRef todo : desc["todos"].toArray())
		{
			m_Todos.insert(todo.toString());
		}

		// changed !
		emit dataChanged();
	}

	//!
	//! Get the url
	//!
	const QString & Blog::GetName(void) const
	{
		return m_Name;
	}

	//!
	//! Get the url
	//!
	QString Blog::GetUrl(void) const
	{
		return QString("http://%1.tumblr.com").arg(m_Name);
	}

	//!
	//! Get the output folder
	//!
	const QString & Blog::GetOutputFolder(void) const
	{
		return m_OutputFolder;
	}

	//!
	//! Set the output folder
	//!
	void Blog::SetOutputFolder(const QString & value)
	{
		if (m_OutputFolder != value)
		{
			m_OutputFolder = value;
			emit dataChanged();
		}
	}

	//!
	//! Get the maximum number of pages scanned during an update
	//!
	int Blog::GetMaxPageUpdate(void) const
	{
		return m_MaxPageUpdate;
	}

	//!
	//! Set the maximum number of pages scanned during an update
	//!
	void Blog::SetMaxPageUpdate(int value)
	{
		if (m_MaxPageUpdate != value)
		{
			m_MaxPageUpdate = value;
			emit dataChanged();
		}
	}

	//!
	//! Get the number of medias.
	//!
	int Blog::GetMediaCount(void) const
	{
		return m_Medias.size();
	}

	//!
	//! Get the number of media with the Todo state
	//!
	int Blog::GetTodoCount(void) const
	{
		return m_Todos.size();
	}

	//!
	//! Get the info on what the blog is currently doing
	//!
	const QString & Blog::GetInfo(void) const
	{
		return m_Info;
	}

	//!
	//! Get the blog's avatar
	//!
	const QImage & Blog::GetAvatar(void) const
	{
		return m_Avatar;
	}

	//!
	//! Set the info on what we're currently doing
	//!
	void Blog::SetInfo(const QString & info)
	{
		if (m_Info != info)
		{
			m_Info = info;
			emit dataChanged();
		}
	}

	//!
	//! Validate a url. Returns an empty string if not valid,
	//! otherwise returns the name of the blog
	//!
	QString Blog::ValidateUrl(const QString & url)
	{
		static QRegExp regexp("http://([^\\.]+)\\.tumblr\\.com/?");
		return regexp.exactMatch(url) == true ? regexp.capturedTexts()[1] : "";
	}

	//!
	//! Update the blog
	//!
	void Blog::Update(void)
	{
		if (m_UpdateMutex.tryLock() == false)
		{
			// an update is already happening, do nothing
			return;
		}

		// start update
		emit dataChanged();
		m_Consecutive = 0;
		m_CancelUpdate = false;

		// get the OAuth values from the settings
		QSettings settings;
		QString oauthKey = settings.value("oauthKey", "").toString();
		QString oauthSecret = settings.value("oauthSecret", "").toString();
		if (oauthKey.isEmpty() == true || oauthSecret.isEmpty() == true)
		{
			m_UpdateMutex.unlock();
			return;
		}

		// setup
		m_TaskManager.PushTask([&](void) {
			// the network manager needs to be created in a task to
			// have the same thread as the tasks doing requests.
			m_NetworkManager = NEW QNetworkAccessManager();
		});

		// update the icon for the blog
		m_TaskManager.PushTask([&](void) {
			// update info
			this->SetInfo("Updating Avatar");

			// request directly as a png
			QByteArray reply = RequestUrl(*m_NetworkManager, QString("http://api.tumblr.com/v2/blog/%1/avatar/40").arg(m_Name));

			// check for cancellation
			if (m_CancelUpdate == true)
			{
				return;
			}

			// update the avatar
			QBuffer buffer(&reply);
			buffer.open(QIODevice::ReadOnly);
			m_Avatar.load(&buffer, "png");

			// notify
			emit dataChanged();
		});

		// update the informations of the blog
		m_TaskManager.PushTask([&, oauthKey](void) {
			// update the blog's info
			this->SetInfo("Scanning For New Medias...");

			// get the new posts
			for (int i = 0; i < m_MaxPageUpdate; ++i)
			{
				// check for cancellation
				if (m_CancelUpdate == true)
				{
					break;
				}

				// get current page
				QString reply = QString::fromUtf8(RequestUrl(
					*m_NetworkManager,
					QString("http://api.tumblr.com/v2/blog/%1/posts?api_key=%2&reblog_info=false&notes_info=false&offset=%3")
						.arg(m_Name)
						.arg(oauthKey)
						.arg(i * 20)
				));

				// check again for cancellation, and errors
				if (m_CancelUpdate == true || reply.isEmpty() == true)
				{
					break;
				}

				// extract medias
				QJsonDocument info(QJsonDocument::fromJson(reply.toUtf8()));
				QJsonArray posts = info.object()["response"].toObject()["posts"].toArray();
				for (QJsonValueRef post : posts)
				{
					QJsonObject postDesc(post.toObject());
					QString type = postDesc["type"].toString();
					if (type == "photo")
					{
						for (QJsonValueRef photo : postDesc["photos"].toArray())
						{
							if (this->AddMedia(photo.toObject()["original_size"].toObject()["url"].toString()) == true)
							{
								return;
							}
						}
					}
					else if (type == "video")
					{
						if (postDesc["video_type"].toString() == "tumblr")
						{
							if (this->AddMedia(postDesc["video_url"].toString()) == true)
							{
								return;
							}
						}
					}
				}
			}
		});

		// download task
		m_TaskManager.PushTask([&](void) {
			// update the blog's info
			this->SetInfo("Downloading Medias...");

			// download
			while (m_Todos.empty() == false)
			{
				// check for cancellation
				if (m_CancelUpdate == true)
				{
					break;
				}

				// get data
				QString url = *m_Todos.begin();
				const QByteArray reply = RequestUrl(*m_NetworkManager, url);

				// detect png hidden in jpg files ...
				const QString extension = url.section('.', -1, -1).toLower();
				if (extension == "jpg" || extension == "jpeg")
				{
					if (reply[0] == static_cast< char >(0x89) && reply[1] == static_cast< char >(0x50))
					{
						url = url.section('.', 0, -2) + ".png";
					}
				}

				// update counts and signal
				m_Medias.insert(*m_Todos.begin());
				m_Todos.erase(m_Todos.begin());
				emit dataChanged();

				// save to disk (dirty hack with extensions ...)
				const QString filename = url.section('/', -1, -1);
				QFile file(m_OutputFolder + "/" + filename);
				if (file.open(QIODevice::WriteOnly) == true)
				{
					file.write(reply);
					file.close();
				}
			}
		});

		// cleanup task
		m_TaskManager.PushTask([&](void) {
			// update the blog's info
			this->SetInfo("");

			// cleanup
			DELETE m_NetworkManager;

			// release the mutex
			m_UpdateMutex.unlock();
			emit dataChanged();
		});
	}

	//!
	//! Cancel an ongoing update
	//!
	void Blog::Cancel(void)
	{
		m_CancelUpdate = true;
	}

	//!
	//! Check if the blog is currently updating
	//!
	bool Blog::IsUpdating(void)
	{
		if (m_UpdateMutex.tryLock() == true)
		{
			m_UpdateMutex.unlock();
			return false;
		}
		return true;
	}

	//!
	//! Remove all the medias from the disk and our lists
	//!
	void Blog::RemoveMedias(void)
	{
		// ensure no update is going on
		QMutexLocker lock(&m_UpdateMutex);

		// and remove the medias
		for (const QString & media : m_Medias)
		{
			QFile(m_OutputFolder + "/" + media.section('/', -1, -1)).remove();
		}
		m_Medias.clear();
	}

	//!
	//! Add a media to the todo list if it doesn't already exists. In addition,
	//! it will count the number of consecutive medias that we already have. If
	//! it goes over a threshold, it means we are scanning old pages, and the
	//! function will return true (meaning "should stop scanning !")
	//!
	bool Blog::AddMedia(const QString & url)
	{
		if (m_Medias.contains(url) == false)
		{
			if (m_Todos.contains(url) == false)
			{
				m_Consecutive = 0;
				m_Todos.insert(url);
				emit dataChanged();
			}

			return false;
		}
		else
		{
			++m_Consecutive;
			return m_Consecutive > MAX_CONSECUTIVE_MEDIAS ? true : false;
		}
	}

	//!
	//! Reset the list of downloaded medias
	//!
	void Blog::reset(void)
	{
		m_Medias.clear();
		m_Todos.clear();
		emit dataChanged();
	}

} // namespace Tumblr
