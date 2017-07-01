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

		// QML API
		Q_INVOKABLE void	updateAll();
		Q_INVOKABLE void	update(int index);
		Q_INVOKABLE void	cancelAll();
		Q_INVOKABLE void	cancel(int index);
		Q_INVOKABLE void	addBlog(const QString & url, const QString & outputFolder);
		Q_INVOKABLE Blog *	blog(int index);
		Q_INVOKABLE void	remove(int index, bool removeMedias);
		Q_INVOKABLE void	importDatabase(const QString & filename);
		Q_INVOKABLE void	exportDatabase(const QString & filename);

	private:

		Blog *	CreateBlog(const QString & name = "", const QString & outputFolder = "");
		QString	GetDatabaseFilename(void) const;

		//! The list of blogs
		QVector< Blog * > m_Blogs;

	};

}


#endif // __TUMBLR_DATABASE_H__
