/*
 * Copyright 2010-2011  OSLL osll@osll.spb.ru
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
/*----------------------------------------------------------------- !
 * PROJ: OSLL/geo2tag
 * ---------------------------------------------------------------- */

#include <qjson/parser.h>
#include <qjson/serializer.h>

#include <QDebug>

#include "GetTimeSlotResponseJSON.h"
#include "JsonUser.h"
#include "JsonChannel.h"
#include "JsonTimeSlot.h"
#include "Channel.h"

GetTimeSlotResponseJSON::GetTimeSlotResponseJSON(QObject *parent) : JsonSerializer(parent)
{
}


QByteArray GetTimeSlotResponseJSON::getJson() const
{
  QJson::Serializer serializer;
  QVariantMap obj;

  QSharedPointer<Channel> channel = m_channelsContainer->at(0);

  obj.insert("errno", m_errno);
  obj.insert("timeSlot", channel->getTimeSlot()->getSlot());
  return serializer.serialize(obj);
}


bool GetTimeSlotResponseJSON::parseJson(const QByteArray &data)
{
  clearContainers();

  QJson::Parser parser;
  bool ok;

  QVariantMap result = parser.parse(data, &ok).toMap();
  if (!ok) return false;

  m_errno = result["errno"].toInt(&ok);
  if (!ok) return false;

  qulonglong slot = result["timeSlot"].toULongLong(&ok);
  if (!ok) return false;

  QSharedPointer<TimeSlot>  timeSlot(new JsonTimeSlot(slot));
  QSharedPointer<Channel> channel(new JsonChannel("unknown", "unknown"));
  channel->setTimeSlot(timeSlot);
  m_channelsContainer->push_back(channel);
  return true;
}