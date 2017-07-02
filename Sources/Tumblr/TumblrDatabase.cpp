#include "TumblrPCH.h"
#include "TumblrDatabase.h"
#include "Utils/Image.h"


namespace Tumblr
{

	//!
	//! Default constructor
	//!
	Database::Database(QObject * parent)
		: QAbstractItemModel(parent)
	{
		this->importDatabase(this->GetDatabaseFilename());
	}

	//!
	//! Default destructor
	//!
	Database::~Database(void)
	{
		this->cancelAll();
		this->exportDatabase(this->GetDatabaseFilename());
	}

	//!
	//! Create a new empty blog and connect its signals
	//!
	Blog * Database::CreateBlog(const QString & name, const QString & outputFolder, int maxPageUpdate)
	{
		Blog * blog = NEW Blog(name, outputFolder);
		blog->SetMaxPageUpdate(maxPageUpdate);

		// todo: optimize this to watch for individual changes, and emit a dataChanged
		// with the correct roles
		QObject::connect(blog, &Blog::dataChanged, [=](void) {
			int index = m_Blogs.indexOf(blog);
			QModelIndex modelIndex(this->createIndex(index, 0, blog));
			emit dataChanged(modelIndex, modelIndex);
		});

		return blog;
	}

	//!
	//! Get the filename of the blogs' database
	//!
	QString	Database::GetDatabaseFilename(void) const
	{
		QSettings settings;
		QString location = settings.value("databaseLocation", "System").toString();
		if (location == "System")
		{
			QStringList locations = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);
			Q_ASSERT(locations.empty() == false);
			QDir().mkpath(locations[0]);
			return locations[0] + "/Blogs.json";
		}
		else if (location == "Application")
		{
			QString str = QCoreApplication::applicationDirPath() + "/Blogs.json";
			return str;
		}
		else
		{
			Q_ASSERT(false);
			return "";
		}
	}

	//!
	//! Update all the blogs
	//!
	void Database::updateAll(void)
	{
		for (Blog * blog : m_Blogs)
		{
			blog->Update();
		}
	}

	//!
	//! Update a blog
	//!
	void Database::update(int index)
	{
		Q_ASSERT(index >= 0 && index < m_Blogs.size());
		m_Blogs[index]->Update();
	}

	//!
	//! Cancel all the blogs' updates
	//!
	void Database::cancelAll(void)
	{
		for (Blog * blog : m_Blogs)
		{
			blog->Cancel();
		}
	}

	//!
	//! Cancel a blog's update
	//!
	void Database::cancel(int index)
	{
		Q_ASSERT(index >= 0 && index < m_Blogs.size());
		m_Blogs[index]->Cancel();
	}

	//!
	//! Add a new blog
	//!
	void Database::addBlog(const QString & url, const QString & outputFolder, int maxPageUpdate)
	{
		// validate
		QString name = Blog::ValidateUrl(url);
		if (name.isEmpty() == true)
		{
			return;
		}

		// check if it already exists
		for (const Blog * blog : m_Blogs)
		{
			if (blog->GetName() == name)
			{
				return;
			}
		}

		// add the new blog
		this->beginInsertRows(QModelIndex(), m_Blogs.size(), m_Blogs.size());
		m_Blogs.push_back(this->CreateBlog(name, outputFolder, maxPageUpdate));
		this->endInsertRows();
	}

	//!
	//! Remove a blog
	//!
	void Database::remove(int index, bool removeMedias)
	{
		Q_ASSERT(index >= 0 && index < m_Blogs.size());
		this->beginRemoveRows(QModelIndex(), index, index);
		if (removeMedias == true)
		{
			m_Blogs[index]->RemoveMedias();
		}
		DELETE m_Blogs[index];
		m_Blogs.remove(index);
		this->endRemoveRows();
	}

	//!
	//! Import an existing database. This will add the informations of
	//! the database to the current one.
	//!
	void Database::importDatabase(const QString & filename)
	{
		QFile file(filename);
		if (file.open(QIODevice::ReadOnly) == true)
		{
			QJsonDocument doc(QJsonDocument::fromJson(file.readAll()));
			for (QJsonValueRef blogDesc : doc.array())
			{
				Blog * blog = this->CreateBlog();
				blog->Deserialize(blogDesc.toObject());

				this->beginInsertRows(QModelIndex(), m_Blogs.size(), m_Blogs.size());
				m_Blogs.push_back(blog);
				this->endInsertRows();
			}
		}
	}

	//!
	//! Export the current database to a file
	//!
	void Database::exportDatabase(const QString & filename)
	{
		// create the Json content
		QJsonArray db;
		for (const Blog * blog : m_Blogs)
		{
			QJsonObject desc;
			blog->Serialize(desc);
			db.append(desc);

			// cleanup
			DELETE blog;
		}

		// write to file
		QFile file(filename);
		if (file.open(QIODevice::WriteOnly) == true)
		{
			QJsonDocument doc(db);
			file.write(doc.toJson());
		}
	}

	//!
	//! Get a blog
	//!
	Blog * Database::blog(int index)
	{
		Q_ASSERT(index >= 0 && index < m_Blogs.size());
		return m_Blogs[index];
	}

	//!
	//! Get the roles supported by this model
	//!
	QHash< int, QByteArray > Database::roleNames(void) const
	{
		QHash< int, QByteArray > roles;
		roles.insert(Qt::DisplayRole,	"name");
		roles.insert(Qt::UserRole,		"url");
		roles.insert(Qt::UserRole + 1,	"outputFolder");
		roles.insert(Qt::UserRole + 2,	"mediaCount");
		roles.insert(Qt::UserRole + 3,	"todoCount");
		roles.insert(Qt::UserRole + 4,	"info");
		roles.insert(Qt::UserRole + 5,	"updating");
		roles.insert(Qt::UserRole + 6,	"avatar");
		return roles;
	}

	//!
	//! Get the data for a given cell and row.
	//!
	QVariant Database::data(const QModelIndex & index, int role) const
	 {
		if (index.isValid() == false)
		{
			return QVariant();
		}

		Blog * blog = static_cast< Blog * >(index.internalPointer());
		Q_ASSERT(blog != nullptr);

		switch (role)
		{
			case Qt::DisplayRole:	return blog->GetName();
			case Qt::UserRole:		return blog->GetUrl();
			case Qt::UserRole + 1:	return blog->GetOutputFolder();
			case Qt::UserRole + 2:	return blog->GetMediaCount();
			case Qt::UserRole + 3:	return blog->GetTodoCount();
			case Qt::UserRole + 4:	return blog->GetInfo();
			case Qt::UserRole + 5:	return blog->IsUpdating();
			case Qt::UserRole + 6:	return ImageToUrl(blog->GetAvatar());
			default:				return QVariant();
		}
	}

	//!
	//! Get an index for a row, column and parent.
	//!
	//! @param row
	//!		Parent relative row index.
	//!
	QModelIndex Database::index(int row, int column, const QModelIndex & parent) const
	{
		Q_UNUSED(parent);
		Q_ASSERT(parent.isValid() == false);
		return this->createIndex(row, column, m_Blogs.at(row));
	}

	//!
	//! Get the parent of a cell.
	//!
	QModelIndex Database::parent(const QModelIndex & /* index */) const
	{
		return QModelIndex();
	}

	//!
	//! Get the number of row (e.g. children) of a cell.
	//!
	int Database::rowCount(const QModelIndex & parent) const
	{
		return parent.isValid() == true ? 0 : m_Blogs.size();
	}

	//!
	//! Get the number of columns.
	//!
	int Database::columnCount(const QModelIndex & /* parent */) const
	{
		return 1;
	}

}
