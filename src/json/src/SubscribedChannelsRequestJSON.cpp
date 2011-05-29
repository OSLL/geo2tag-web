/*
 * Copyright 2010  Open Source & Linux Lab (OSLL)  osll@osll.spb.ru
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * The advertising clause requiring mention in adverts must never be included.
 */

/*! ---------------------------------------------------------------
 * $Id$
 *
 * \file SubscribedChannelsJSON.cpp
 * \brief SubscribedChannelsJSON implementation
 *
 * File description
 *
 * PROJ: OSLL/geo2tag
 * ---------------------------------------------------------------- */

#include <qjson/parser.h>
#include <qjson/serializer.h>

#include <QVariant>
#include <QVariantMap>

#include "SubscribedChannelsRequestJSON.h"
#include "JsonChannel.h"
#include "JsonDataMark.h"
#include "JsonUser.h"
#include <syslog.h>
SubscribedChannelsRequestJSON::SubscribedChannelsRequestJSON()
{
  syslog(LOG_INFO,"SubscribedChannelsRequestJSON::SubscribedChannelsRequestJSON()");
}


SubscribedChannelsRequestJSON::SubscribedChannelsRequestJSON(const QSharedPointer<User> &user)
{
  m_usersContainer->push_back(user);
}


void SubscribedChannelsRequestJSON::parseJson(const QByteArray &data)
{
  clearContainers();

  QJson::Parser parser;
  bool ok;
  QVariantMap result = parser.parse(data, &ok).toMap();

  if (!ok)
  {
    qFatal("An error occured during parsing json with SubscribedChannelsReques");
    return;
  }
  QString authToken =    result["auth_token"].toString();
  QSharedPointer<User>    dummyUser(new JsonUser("unknown", "unknown", authToken));
  m_usersContainer->push_back(dummyUser);
}


QByteArray SubscribedChannelsRequestJSON::getJson() const
{
  QJson::Serializer serializer;
  QVariantMap request;

  request.insert("auth_token", m_usersContainer->at(0)->getToken());

  return serializer.serialize(request);
}


/* ===[ End of file $HeadURL$ ]=== */