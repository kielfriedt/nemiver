// Author: Fabien Parent
/*
 *This file is part of the Nemiver project
 *
 *Nemiver is free software; you can redistribute
 *it and/or modify it under the terms of
 *the GNU General Public License as published by the
 *Free Software Foundation; either version 2,
 *or (at your option) any later version.
 *
 *Nemiver is distributed in the hope that it will
 *be useful, but WITHOUT ANY WARRANTY;
 *without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *See the GNU General Public License for more details.
 *
 *You should have received a copy of the
 *GNU General Public License along with Nemiver;
 *see the file COPYING.
 *If not, write to the Free Software Foundation,
 *Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *See COPYRIGHT file copyright information.
 */

#include "nmv-confmgr-record-options.h"
#include "nmv-conf-keys.h"
#include "nmv-i-conf-mgr.h"

NEMIVER_BEGIN_NAMESPACE (nemiver)

struct ConfMgrRecordOptions::Priv {
    IConfMgr &conf_mgr;

    Priv (IConfMgr &a_confmgr) :
        conf_mgr (a_confmgr)
    {
    }
};

ConfMgrRecordOptions::ConfMgrRecordOptions (IConfMgr &a_confmgr) :
    m_priv (new Priv (a_confmgr))
{
}

ConfMgrRecordOptions::~ConfMgrRecordOptions ()
{
}

bool
ConfMgrRecordOptions::do_callgraph_recording () const
{
    bool callgraph_recording = false;
    if (!m_priv->conf_mgr.get_key_value (CONF_KEY_DO_CALLGRAPH_RECORDING,
                                         callgraph_recording)) {
        LOG_ERROR ("failed to get gconf key "
                   << CONF_KEY_DO_CALLGRAPH_RECORDING);
    }
    return callgraph_recording;
}

bool
ConfMgrRecordOptions::do_collect_without_buffering () const
{
    bool collect_without_buffering = false;
    if (!m_priv->conf_mgr.get_key_value (CONF_KEY_COLLECT_WITHOUT_BUFFERING,
                                         collect_without_buffering)) {
        LOG_ERROR ("failed to get gconf key "
                   << CONF_KEY_COLLECT_WITHOUT_BUFFERING);
    }
    return collect_without_buffering;
}

bool
ConfMgrRecordOptions::do_collect_raw_sample_records () const
{
    bool collect_raw_sample_records = false;
    if (!m_priv->conf_mgr.get_key_value (CONF_KEY_COLLECT_RAW_SAMPLE_RECORDS,
                                         collect_raw_sample_records)) {
        LOG_ERROR ("failed to get gconf key "
                   << CONF_KEY_COLLECT_RAW_SAMPLE_RECORDS);
    }
    return collect_raw_sample_records;
}

bool
ConfMgrRecordOptions::do_system_wide_collection () const
{
    bool system_wide_collection = false;
    if (!m_priv->conf_mgr.get_key_value (CONF_KEY_SYSTEM_WIDE_COLLECTION,
                                         system_wide_collection)) {
        LOG_ERROR ("failed to get gconf key "
                   << CONF_KEY_SYSTEM_WIDE_COLLECTION);
    }
    return system_wide_collection;
}

bool
ConfMgrRecordOptions::do_sample_addresses () const
{
    bool sample_addresses = false;
    if (!m_priv->conf_mgr.get_key_value (CONF_KEY_SAMPLE_ADDRESSES,
                                         sample_addresses)) {
        LOG_ERROR ("failed to get gconf key "
                   << CONF_KEY_SAMPLE_ADDRESSES);
    }
    return sample_addresses;
}

bool
ConfMgrRecordOptions::do_sample_timestamps () const
{
    bool sample_timestamps = false;
    if (!m_priv->conf_mgr.get_key_value (CONF_KEY_SAMPLE_TIMESTAMPS,
                                         sample_timestamps)) {
        LOG_ERROR ("failed to get gconf key "
                   << CONF_KEY_SAMPLE_TIMESTAMPS);
    }
    return sample_timestamps;
}

NEMIVER_END_NAMESPACE (nemiver)

