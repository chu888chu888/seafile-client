extern "C" {

#include <ccnet/peer.h>

}

#include <QTimer>

#include "QtAwesome.h"
#include "seafile-applet.h"
#include "rpc/rpc-client.h"
#include "server-status-dialog.h"


namespace {

const int kRefreshStatusInterval = 1000; // 1 sec

} // namespace

ServerStatusDialog::ServerStatusDialog(QWidget *parent) : QDialog(parent)
{
    setupUi(this);

    setWindowTitle(tr("Servers connection status"));

    refresh_timer_ = new QTimer(this);
    connect(refresh_timer_, SIGNAL(timeout()), this, SLOT(refreshStatus()));

    refreshStatus();
    refresh_timer_->start(kRefreshStatusInterval);
}

void ServerStatusDialog::refreshStatus()
{
    GList *servers = NULL;
    if (seafApplet->rpcClient()->getServers(&servers) < 0) {
        qDebug("failed to get ccnet servers list\n");
        return;
    }

    mList->clear();

    if (!servers) {
        return;
    }

    GList *ptr;
    for (ptr = servers; ptr ; ptr = ptr->next) {
        CcnetPeer *server = (CcnetPeer *)ptr->data;
        QString name;
        name.sprintf("%s", server->public_addr);

        QListWidgetItem *item = new QListWidgetItem(mList);
        item->setData(Qt::DisplayRole, name);

        if (server->net_state == PEER_CONNECTED) {
            item->setData(Qt::DecorationRole, QIcon(":/images/sync/ok.png"));
            item->setData(Qt::ToolTipRole, tr("connected"));
        } else {
            item->setData(Qt::DecorationRole, awesome->icon(icon_remove, QColor("red")));
            item->setData(Qt::ToolTipRole, tr("disconnected"));
        }

        mList->addItem(item);
    }

    g_list_foreach (servers, (GFunc)g_object_unref, NULL);
    g_list_free (servers);
}


