/*
 *  This file is part of choqok-sina
 *  Copyright (C) 2011 Ni Hui <shuizhuyuanluo@126.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License or (at your option) version 3 or any later version
 *  accepted by the membership of KDE e.V. (or its successor approved
 *  by the membership of KDE e.V.), which shall act as a proxy
 *  defined in Section 14 of version 3 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "fanfoupostwidget.h"
#include "fanfouaccount.h"
#include "fanfoumicroblog.h"

#include <choqok/mediamanager.h>

#include <KAction>
#include <KLocale>
#include <KMenu>
#include <KPushButton>
#include <QRegExp>

static const QRegExp UserRegExp("@([^\\s\\W]+)", Qt::CaseInsensitive);
static const QRegExp TagRegExp("#([^#]+)#", Qt::CaseInsensitive);

static const KIcon unFavIcon( Choqok::MediaManager::convertToGrayScale( KIcon( "rating" ).pixmap( 16 ) ) );

FanfouPostWidget::FanfouPostWidget( FanfouAccount* account, const Choqok::Post& post, QWidget* parent )
: Choqok::UI::PostWidget(account,post,parent)
{
    FanfouMicroBlog* microblog = dynamic_cast<FanfouMicroBlog*>(account->microblog());
    connect( microblog, SIGNAL(favoriteRemoved(Choqok::Account*,Choqok::Post*)),
             this, SLOT(slotFavoriteRemoved(Choqok::Account*,Choqok::Post*)) );
    connect( microblog, SIGNAL(favoriteCreated(Choqok::Account*,Choqok::Post*)),
             this, SLOT(slotFavoriteCreated(Choqok::Account*,Choqok::Post*)) );

}

FanfouPostWidget::~FanfouPostWidget()
{
}

void FanfouPostWidget::initUi()
{
    Choqok::UI::PostWidget::initUi();

    KPushButton* replyButton = addButton( "replyButton", i18n( "Reply" ), "edit-undo" );
    connect( replyButton, SIGNAL(clicked(bool)), this, SLOT(slotReply()) );

    KMenu* menu = new KMenu;
    KAction* replyAction = new KAction( KIcon( "edit-undo" ), i18n( "Reply to %1", currentPost().author.userName ), menu );
    connect( replyAction, SIGNAL(triggered(bool)), this, SLOT(slotReply()) );
    menu->addAction( replyAction );
    KAction* writeAction = new KAction( KIcon( "document-edit" ), i18n( "Write to %1", currentPost().author.userName ), menu );
    connect( writeAction, SIGNAL(triggered(bool)), this, SLOT(slotWrite()) );
    menu->addAction( writeAction );
    if( !currentPost().isPrivate ) {
        KAction* replyAllAction = new KAction( i18n( "Reply to all" ), menu );
        connect( replyAllAction, SIGNAL(triggered(bool)), this, SLOT(slotReplyAll()) );
        menu->addAction(replyAllAction);
    }

    menu->setDefaultAction( replyAction );
    replyButton->setDelayedMenu( menu );

    if( !currentPost().isPrivate ) {
        favoriteButton = addButton( "favoriteButton",i18n( "Favorite" ), "rating" );
        favoriteButton->setCheckable( true );
        connect( favoriteButton, SIGNAL(clicked(bool)), this, SLOT(slotFavorite()) );
        if ( currentPost().isFavorited ) {
            favoriteButton->setChecked( true );
            favoriteButton->setIcon( KIcon( "rating" ) );
        }
        else {
            favoriteButton->setChecked( false );
            favoriteButton->setIcon( unFavIcon );
        }
    }
}

QString FanfouPostWidget::prepareStatus(const QString& text)
{
    QString res = Choqok::UI::PostWidget::prepareStatus(text);
    res.replace(UserRegExp,"@<a href='user://\\1'>\\1</a>");
    res.replace(TagRegExp,"#<a href='tag://\\1'>\\1</a>");
    return res;
}

void FanfouPostWidget::slotResendPost()
{
    setReadWithSignal();
    FanfouMicroBlog* microblog = dynamic_cast<FanfouMicroBlog*>(currentAccount()->microblog());
    Choqok::Post* post = new Choqok::Post;
    post->postId = currentPost().postId;
    microblog->retweetPost( currentAccount(), post );
}

void FanfouPostWidget::slotReply()
{
    setReadWithSignal();
    if (currentPost().isPrivate) {
	//FanfouMicroBlog *microblog = qobject_cast<FanfouMicroBlog*>(currentAccount()->microblog());
	
    } else {
	QString replyTo = QString("@%1").arg( currentPost().author.userName );
	QString postId = currentPost().postId;
	QString userName = currentPost().author.userName;
	
	if( !currentPost().repeatedFromUsername.isEmpty() ){
            replyTo.prepend(QString("@%1 ").arg(currentPost().repeatedFromUsername));
            postId = currentPost().repeatedPostId;
        }
	
	emit reply( replyTo, postId, userName );
    }
  
}

void FanfouPostWidget::slotWrite()
{
    emit reply( QString("@%1").arg( currentPost().author.userName ), QString(), currentPost().author.userName );
}

void FanfouPostWidget::slotReplyAll()
{
    QString txt = QString("@%1").arg( currentPost().author.userName );
    emit reply( txt, currentPost().postId, currentPost().author.userName );
}

void FanfouPostWidget::slotFavorite()
{
    setReadWithSignal();
    FanfouMicroBlog* microblog = dynamic_cast<FanfouMicroBlog*>(currentAccount()->microblog());
    Choqok::Post* post = const_cast<Choqok::Post*>(&currentPost());/// NOTE use dummy or modeling one? --- nihui
//     post->postId = currentPost().postId;
    if ( currentPost().isFavorited )
        microblog->removeFavorite( currentAccount(), post );
    else
        microblog->createFavorite( currentAccount(), post );
}

void FanfouPostWidget::slotFavoriteCreated( Choqok::Account* account, Choqok::Post* post )
{
    if ( currentAccount() != account || post->postId != currentPost().postId )
        return;

//     delete post;
    Choqok::Post tmp = currentPost();
    tmp.isFavorited = true;
    setCurrentPost( tmp );
    favoriteButton->setChecked( true );
    favoriteButton->setIcon( KIcon( "rating" ) );
}

void FanfouPostWidget::slotFavoriteRemoved( Choqok::Account* account, Choqok::Post* post )
{
    if ( currentAccount() != account || post->postId != currentPost().postId )
        return;

//     delete post;
    Choqok::Post tmp = currentPost();
    tmp.isFavorited = false;
    setCurrentPost( tmp );
    favoriteButton->setChecked( false );
    favoriteButton->setIcon( unFavIcon );
}
