/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "IVideoInfoTagLoader.h"
#include "utils/Artwork.h"
#include "video/VideoInfoTag.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

//! \brief Video tag loader from plugin source.
class CVideoTagLoaderPlugin : public KODI::VIDEO::IVideoInfoTagLoader
{
public:
  CVideoTagLoaderPlugin(const CFileItem& item, bool forceRefresh);

  ~CVideoTagLoaderPlugin() override = default;

  //! \brief Returns whether or not read has info.
  bool HasInfo() const override;

  //! \brief Load "tag" from plugin.
  //! \param tag Tag to load info into
  CInfoScanner::InfoType Load(CVideoInfoTag& tag,
                              bool prioritise,
                              std::vector<EmbeddedArt>* = nullptr) override;

  inline std::unique_ptr<KODI::ART::Artwork>& GetArt() { return m_art; }

protected:
  std::unique_ptr<CVideoInfoTag> m_tag;
  std::unique_ptr<KODI::ART::Artwork> m_art;
  bool m_force_refresh;
};
