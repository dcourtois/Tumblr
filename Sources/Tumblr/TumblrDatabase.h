#ifndef __TUMBLR_DATABASE_H__
#define __TUMBLR_DATABASE_H__


#include "TumblrBlog.h"


namespace Tumblr
{

	//!
	//! Stores Tumblr objects
	//!
	class Database
		: public QAbstractItemModel
	{

		Q_OBJECT

		Q_PROPERTY(QString location READ GetLocation WRITE SetLocation NOTIFY locationChanged)
		Q_PROPERTY(QString secret READ GetSecret WRITE SetSecret NOTIFY secretChanged)
		Q_PROPERTY(QString key READ GetKey WRITE SetKey NOTIFY keyChanged)

	signals:

		void	locationChanged(const QString & value);
		void	secretChanged(const QString & value);
		void	keyChanged(const QString & value);

	public:

		Database(QObject * parent = nullptr);
		~Database(void);

		// QAbstractItemModel implementation
		QHash< int, QByteArray >	roleNames(void) const final;
		QVariant					data(const QModelIndex & index, int role = Qt::DisplayRole) const final;
		QModelIndex					index(int row, int column, const QModelIndex & parent = QModelIndex()) const final;
		QModelIndex					parent(const QModelIndex & index) const final;
		int							rowCount(const QModelIndex & parent = QModelIndex()) const final;
		int							columnCount(const QModelIndex & parent = QModelIndex()) const final;

		// C++ API
		const QString &		GetLocation(void) const;
		void				SetLocation(const QString & location);
		const QString &		GetSecret(void) const;
		void				SetSecret(const QString & secret);
		const QString &		GetKey(void) const;
		void				SetKey(const QString & key);

		// QML API
		Q_INVOKABLE void	updateAll();
		Q_INVOKABLE void	update(int index);
		Q_INVOKABLE void	cancelAll();
		Q_INVOKABLE void	cancel(int index);
		Q_INVOKABLE void	addBlog(const QString & url, const QString & outputFolder, int maxPageUpdate);
		Q_INVOKABLE Blog *	blog(int index);
		Q_INVOKABLE void	remove(int index, bool removeMedias);
		Q_INVOKABLE void	importDatabase(const QString & filename);
		Q_INVOKABLE void	exportDatabase(const QString & filename);

	private:

		Blog *	CreateBlog(const QString & name = "", const QString & outputFolder = "", int maxPageUpdate = 10);
		QString	GetDatabaseFilename(void) const;

		//! The list of blogs
		QVector< Blog * > m_Blogs;

		//! Database location
		QString m_Location;

		//! Auth secret
		QString m_Secret;

		//! Auth key
		QString m_Key;

	};

}


#endif // __TUMBLR_DATABASE_H__
