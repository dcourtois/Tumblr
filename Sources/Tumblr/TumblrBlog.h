#ifndef ___BLOG_H__
#define ___BLOG_H__


#include "Utils/TaskManager.h"


namespace Tumblr
{

	//!
	//! Stores a blog
	//!
	class Blog
		: public QObject
	{

		Q_OBJECT

		// note : dataChanged is quick and dirty way to notify the database that
		// we changed, and it's emitted a lot. When performance problems arise,
		// split signals.
		Q_PROPERTY(QString name READ GetName NOTIFY dataChanged)
		Q_PROPERTY(QString url READ GetUrl NOTIFY dataChanged)
		Q_PROPERTY(int mediaCount READ GetMediaCount NOTIFY dataChanged)
		Q_PROPERTY(QImage avatar READ GetAvatar NOTIFY dataChanged)
		Q_PROPERTY(QString output READ GetOutputFolder WRITE SetOutputFolder NOTIFY dataChanged)

	signals:

		void	dataChanged(void);

	public:

		Blog(void);
		Blog(const Blog & other);
		Blog(const QString & name);
		Blog(const QString & name, const QString & outputFolder);
		~Blog(void);

		// C++ API
		void					Serialize(QJsonObject & desc) const;
		void					Deserialize(const QJsonObject & desc);
		const QString &			GetName(void) const;
		QString					GetUrl(void) const;
		const QString &			GetOutputFolder(void) const;
		void					SetOutputFolder(const QString & value);
		int						GetMediaCount(void) const;
		int						GetTodoCount(void) const;
		const QString &			GetInfo(void) const;
		const QImage &			GetAvatar(void) const;
		void					Update(void);
		void					Cancel(void);
		bool					IsUpdating(void);
		void					RemoveMedias(void);
		static QString			ValidateUrl(const QString & url);

	private:

		bool	AddMedia(const QString & url);
		void	SetInfo(const QString & info);

		//! The blog name
		QString m_Name;

		//! The output folder
		QString m_OutputFolder;

		//! The list medias to download
		QSet< QString > m_Todos;

		//! The list of medias
		QSet< QString > m_Medias;

		//! What am I doing ??
		QString m_Info;

		//! the icon for the blog
		QImage m_Avatar;

		//! Temporary task manager used to update the blog on another thread
		TaskManager m_TaskManager;

		//! Network manager used during update (needs to be created on the thread
		//! doing the requests)
		QNetworkAccessManager * m_NetworkManager;

		//! Mutex used to protect the update
		QMutex m_UpdateMutex;

		//! When true, we must stop updating !
		bool m_CancelUpdate;

		//! Used to check the number of consecutive images that we already have in store.
		//! This is used to avoid asking for old pages and wasting requests.
		int m_Consecutive;

	};

} // namespace Tumblr


#endif // ___BLOG_H__
