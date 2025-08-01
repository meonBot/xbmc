/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "GUIListItem.h"

#include "GUIListItemLayout.h"
#include "utils/Archive.h"
#include "utils/CharsetConverter.h"
#include "utils/StringUtils.h"
#include "utils/Variant.h"

#include <utility>

bool CGUIListItem::CaseInsensitiveCompare::operator()(const std::string_view& s1,
                                                      const std::string_view& s2) const
{
  return StringUtils::CompareNoCase(s1, s2) < 0;
}

CGUIListItem::CGUIListItem(const CGUIListItem& item)
{
  *this = item;
  SetInvalid();
}

CGUIListItem::CGUIListItem() = default;

CGUIListItem::CGUIListItem(const std::string& strLabel) : m_strLabel(strLabel)
{
  SetSortLabel(strLabel);
}

CGUIListItem::~CGUIListItem()
{
  FreeMemory();
}

void CGUIListItem::SetLabel(const std::string& strLabel)
{
  if (m_strLabel == strLabel)
    return;
  m_strLabel = strLabel;
  if (m_sortLabel.empty())
    SetSortLabel(strLabel);
  SetInvalid();
}

const std::string& CGUIListItem::GetLabel() const
{
  return m_strLabel;
}

void CGUIListItem::SetLabel2(std::string_view strLabel2)
{
  if (m_strLabel2 == strLabel2)
    return;
  m_strLabel2 = strLabel2;
  SetInvalid();
}

const std::string& CGUIListItem::GetLabel2() const
{
  return m_strLabel2;
}

void CGUIListItem::SetSortLabel(const std::string &label)
{
  CCharsetConverter::utf8ToW(label, m_sortLabel, false);
  // no need to invalidate - this is never shown in the UI
}

void CGUIListItem::SetSortLabel(const std::wstring &label)
{
  m_sortLabel = label;
}

const std::wstring& CGUIListItem::GetSortLabel() const
{
  return m_sortLabel;
}

void CGUIListItem::SetArt(const std::string& type, std::string_view url)
{
  const auto i = m_art.find(type);
  if (i == m_art.end() || i->second != url)
  {
    m_art[type] = url;
    SetInvalid();
  }
}

void CGUIListItem::SetArt(const KODI::ART::Artwork& art)
{
  m_art = art;
  SetInvalid();
}

void CGUIListItem::SetArtFallback(const std::string& from, std::string_view to)
{
  m_artFallbacks[from] = to;
}

void CGUIListItem::ClearArt()
{
  m_art.clear();
  m_artFallbacks.clear();
  SetProperty("libraryartfilled", false);
}

void CGUIListItem::AppendArt(const KODI::ART::Artwork& art, const std::string& prefix)
{
  for (const auto& [type, url] : art)
    SetArt(prefix.empty() ? type : prefix + '.' + type, url);
}

std::string CGUIListItem::GetArt(const std::string &type) const
{
  auto i = m_art.find(type);
  if (i != m_art.end())
    return i->second;

  i = m_artFallbacks.find(type);
  if (i != m_artFallbacks.end())
  {
    const auto j = m_art.find(i->second);
    if (j != m_art.end())
      return j->second;
  }
  return "";
}

const KODI::ART::Artwork& CGUIListItem::GetArt() const
{
  return m_art;
}

bool CGUIListItem::HasArt(const std::string &type) const
{
  return !GetArt(type).empty();
}

void CGUIListItem::SetOverlayImage(GUIIconOverlay icon)
{
  if (m_overlayIcon == icon)
    return;

  m_overlayIcon = icon;
  SetInvalid();
}

std::string CGUIListItem::GetOverlayImage() const
{
  switch (m_overlayIcon)
  {
  case ICON_OVERLAY_RAR:
    return "OverlayRAR.png";
  case ICON_OVERLAY_ZIP:
    return "OverlayZIP.png";
  case ICON_OVERLAY_LOCKED:
    return "OverlayLocked.png";
  case ICON_OVERLAY_UNWATCHED:
    return "OverlayUnwatched.png";
  case ICON_OVERLAY_WATCHED:
    return "OverlayWatched.png";
  case ICON_OVERLAY_HD:
    return "OverlayHD.png";
  default:
    return "";
  }
}

void CGUIListItem::Select(bool bOnOff)
{
  m_bSelected = bOnOff;
}

bool CGUIListItem::HasOverlay() const
{
  return (m_overlayIcon != CGUIListItem::ICON_OVERLAY_NONE);
}

bool CGUIListItem::IsSelected() const
{
  return m_bSelected;
}

CGUIListItem& CGUIListItem::operator =(const CGUIListItem& item)
{
  if (&item == this)
    return *this;

  m_strLabel2 = item.m_strLabel2;
  m_strLabel = item.m_strLabel;
  m_sortLabel = item.m_sortLabel;
  FreeMemory();
  m_bSelected = item.m_bSelected;
  m_overlayIcon = item.m_overlayIcon;
  m_bIsFolder = item.m_bIsFolder;
  m_mapProperties = item.m_mapProperties;
  m_art = item.m_art;
  m_artFallbacks = item.m_artFallbacks;
  m_currentItem = item.m_currentItem;
  SetInvalid();
  return *this;
}

void CGUIListItem::Archive(CArchive &ar)
{
  if (ar.IsStoring())
  {
    ar << m_bIsFolder;
    ar << m_strLabel;
    ar << m_strLabel2;
    ar << m_sortLabel;
    ar << m_bSelected;
    ar << m_overlayIcon;
    ar << static_cast<int>(m_mapProperties.size());
    for (const auto& [name, value] : m_mapProperties)
    {
      ar << name;
      ar << value;
    }
    ar << static_cast<int>(m_art.size());
    for (const auto& [type, url] : m_art)
    {
      ar << type;
      ar << url;
    }
    ar << static_cast<int>(m_artFallbacks.size());
    for (const auto& [type, url] : m_artFallbacks)
    {
      ar << type;
      ar << url;
    }
    ar << m_currentItem;
  }
  else
  {
    ar >> m_bIsFolder;
    ar >> m_strLabel;
    ar >> m_strLabel2;
    ar >> m_sortLabel;
    ar >> m_bSelected;

    int overlayIcon;
    ar >> overlayIcon;
    m_overlayIcon = GUIIconOverlay(overlayIcon);

    int mapSize;
    ar >> mapSize;
    for (int i = 0; i < mapSize; i++)
    {
      std::string key;
      CVariant value;
      ar >> key;
      ar >> value;
      SetProperty(key, value);
    }
    ar >> mapSize;
    for (int i = 0; i < mapSize; i++)
    {
      std::string key;
      std::string value;
      ar >> key;
      ar >> value;
      m_art.try_emplace(key, value);
    }
    ar >> mapSize;
    for (int i = 0; i < mapSize; i++)
    {
      std::string key;
      std::string value;
      ar >> key;
      ar >> value;
      m_artFallbacks.try_emplace(key, value);
    }
    ar >> m_currentItem;
    SetInvalid();
  }
}
void CGUIListItem::Serialize(CVariant& value) const
{
  value["isFolder"] = m_bIsFolder;
  value["strLabel"] = m_strLabel;
  value["strLabel2"] = m_strLabel2;
  value["sortLabel"] = m_sortLabel;
  value["selected"] = m_bSelected;

  for (const auto& [propname, propvalue] : m_mapProperties)
    value["properties"][propname] = propvalue;

  for (const auto& [type, url] : m_art)
    value["art"][type] = url;

  value["current"] = m_currentItem;
}

void CGUIListItem::FreeIcons()
{
  FreeMemory();
  ClearArt();
  SetInvalid();
}

void CGUIListItem::FreeMemory(bool immediately)
{
  if (m_layout)
  {
    m_layout->FreeResources(immediately);
    m_layout.reset();
  }
  if (m_focusedLayout)
  {
    m_focusedLayout->FreeResources(immediately);
    m_focusedLayout.reset();
  }
}

void CGUIListItem::SetLayout(std::unique_ptr<CGUIListItemLayout> layout)
{
  m_layout = std::move(layout);
}

CGUIListItemLayout *CGUIListItem::GetLayout()
{
  return m_layout.get();
}

void CGUIListItem::SetFocusedLayout(std::unique_ptr<CGUIListItemLayout> layout)
{
  m_focusedLayout = std::move(layout);
}

CGUIListItemLayout *CGUIListItem::GetFocusedLayout()
{
  return m_focusedLayout.get();
}

void CGUIListItem::SetInvalid()
{
  if (m_layout)
    m_layout->SetInvalid();

  if (m_focusedLayout)
    m_focusedLayout->SetInvalid();
}

void CGUIListItem::SetProperty(const std::string &strKey, const CVariant &value)
{
  const auto iter = m_mapProperties.find(strKey);
  if (iter == m_mapProperties.end())
  {
    m_mapProperties.try_emplace(strKey, value);
    SetInvalid();
  }
  else if (iter->second != value)
  {
    iter->second = value;
    SetInvalid();
  }
}

const CVariant &CGUIListItem::GetProperty(const std::string &strKey) const
{
  static CVariant nullVariant{CVariant::VariantTypeNull};

  const auto iter = m_mapProperties.find(strKey);
  if (iter == m_mapProperties.end())
    return nullVariant;

  return iter->second;
}

bool CGUIListItem::HasProperty(const std::string &strKey) const
{
  return m_mapProperties.contains(strKey);
}

void CGUIListItem::ClearProperty(const std::string &strKey)
{
  const auto iter = m_mapProperties.find(strKey);
  if (iter != m_mapProperties.end())
  {
    m_mapProperties.erase(iter);
    SetInvalid();
  }
}

void CGUIListItem::ClearProperties()
{
  if (!m_mapProperties.empty())
  {
    m_mapProperties.clear();
    SetInvalid();
  }
}

void CGUIListItem::IncrementProperty(const std::string &strKey, int nVal)
{
  int64_t i = GetProperty(strKey).asInteger();
  i += nVal;
  SetProperty(strKey, i);
}

void CGUIListItem::IncrementProperty(const std::string& strKey, int64_t nVal)
{
  int64_t i = GetProperty(strKey).asInteger();
  i += nVal;
  SetProperty(strKey, i);
}

void CGUIListItem::IncrementProperty(const std::string &strKey, double dVal)
{
  double d = GetProperty(strKey).asDouble();
  d += dVal;
  SetProperty(strKey, d);
}

void CGUIListItem::AppendProperties(const CGUIListItem &item)
{
  for (const auto& [propname, propvalue] : item.m_mapProperties)
    SetProperty(propname, propvalue);
}

void CGUIListItem::SetProperties(const PropertyMap& props)
{
  m_mapProperties = props;
}

void CGUIListItem::SetCurrentItem(unsigned int position)
{
  m_currentItem = position;
}

unsigned int CGUIListItem::GetCurrentItem() const
{
  return m_currentItem;
}
