// -*- coding: utf-8 -*-
// Copyright (C) 2022 Rosen Diankov <rosen.diankov@gmail.com>
//
// This file is part of OpenRAVE.
// OpenRAVE is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
/** \file jsondownloader.h
    \brief Helper class for jsonreader to download files remotely
 */

#ifndef OPENRAVE_JSON_DOWNLOADER_H
#define OPENRAVE_JSON_DOWNLOADER_H

#include "jsoncommon.h"

#if OPENRAVE_CURL

#include <curl/curl.h>

namespace OpenRAVE {

/// \brief Context of a download, passed to curl handler to buffer incoming data
struct JSONDownloadContext
{
public:
    JSONDownloadContext();
    ~JSONDownloadContext();

    std::string uri; ///< canonicalized uri of the download
    CURL* curl = nullptr; ///< the curl handle for this download
    rapidjson::StringBuffer buffer; ///< buffer used to receive downloaded data
    rapidjson::Document* pDoc = nullptr; ///< if non-null, the caller-supplied document to put results into
    uint64_t startTimestampUS = 0; ///< start timestamp in microseconds
};
typedef boost::shared_ptr<JSONDownloadContext> JSONDownloadContextPtr;

/// \brief Downloader to download one or multiple uris and their references, used by JSONDownloaderScope to share keep-alive connections and other resources
class JSONDownloader {
public:
    JSONDownloader(std::map<std::string, boost::shared_ptr<const rapidjson::Document> >& rapidJSONDocuments, const std::vector<std::string>& vOpenRAVESchemeAliases, const std::string& remoteUrl, const std::string& unixEndpoint);
    ~JSONDownloader();

protected:
    std::map<std::string, boost::shared_ptr<const rapidjson::Document> >& _rapidJSONDocuments; ///< cache for opened rapidjson Documents, newly downloaded documents will be inserted here, passed in via constructor
    const std::vector<std::string>& _vOpenRAVESchemeAliases; ///< list of scheme aliases, passed in via constructor
    const std::string _remoteUrl; ///< remote url for scheme, passed in via constructor
    const std::string _unixEndpoint; ///< unix endpoint for establishing unix domain socket instead of tcp socket

    CURLM* _curlm = nullptr; ///< curl multi handler, used to downlod files simultaneously
    std::string _userAgent; ///< user agent to use when downloading from server

    std::vector<JSONDownloadContextPtr> _vDownloadContextPool; ///< pool of JSONDownloadContext objects to be reused

    friend class JSONDownloaderScope;
};
typedef boost::shared_ptr<JSONDownloader> JSONDownloaderPtr;


/// \brief Downloader to be used in scope to download one or multiple uris and their references.
class JSONDownloaderScope {

public:
    JSONDownloaderScope(JSONDownloader& downloader, rapidjson::Document::AllocatorType& alloc, bool downloadRecursively = true);
    ~JSONDownloaderScope();

    /// \brief Download one uri into supplied doc
    /// \param uri URI to download
    /// \param doc rapidjson document to store the downloaded and parsed document
    /// \param timeoutUS timeout in microseconds to wait for download to finish
    void Download(const std::string& uri, rapidjson::Document& doc, uint64_t timeoutUS = 10000000);

    /// \brief Queues uri to download
    /// \param uri URI to download
    void QueueDownloadURI(const std::string& uri) {
        _QueueDownloadURI(uri, nullptr);
    }

    /// \brief Downloads all reference uris in the supplied env info, as well as all their further references
    /// \param rEnvInfo env info where reference uris should be discovered
    void QueueDownloadReferenceURIs(const rapidjson::Value& rEnvInfo);

    /// \brief Wait for queued downloads to finish, downloaded documents are inserted into rapidJSONDocuments passed in constructor
    /// \param timeoutUS timeout in microseconds to wait for download to finish
    void WaitForDownloads(uint64_t timeoutUS = 10000000);

protected: 

    /// \brief Queue uri to be downloaded, optionally supply a rapidjson document to be used to store result
    void _QueueDownloadURI(const std::string& uri, rapidjson::Document* pDoc);

    /// \brief Returns true if the referenceUri is a valid URI that can be loaded
    bool _IsExpandableReferenceUri(const std::string& referenceUri) const;

    JSONDownloader& _downloader; ///< reference to JSONDownloader instance

    rapidjson::Document::AllocatorType& _alloc; ///< re-use allocator, passed in via constructor

    bool _downloadRecursively = true; ///< whether to recurse all referenced uris

    std::map<CURL*, JSONDownloadContextPtr> _mapDownloadContexts; ///< map from curl handle to download context
};

}

#endif // OPENRAVE_CURL

#endif
