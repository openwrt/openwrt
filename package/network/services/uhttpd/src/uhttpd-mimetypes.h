/*
 * uhttpd - Tiny single-threaded httpd - MIME type definitions
 *
 *   Copyright (C) 2010 Jo-Philipp Wich <xm@subsignal.org>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef _UHTTPD_MIMETYPES_

static struct mimetype uh_mime_types[] = {

	{ "txt",     "text/plain" },
	{ "log",     "text/plain" },
	{ "js",      "text/javascript" },
	{ "css",     "text/css" },
	{ "htm",     "text/html" },
	{ "html",    "text/html" },
	{ "diff",    "text/x-patch" },
	{ "patch",   "text/x-patch" },
	{ "c",       "text/x-csrc" },
	{ "h",       "text/x-chdr" },
	{ "o",       "text/x-object" },
	{ "ko",      "text/x-object" },

	{ "bmp",     "image/bmp" },
	{ "gif",     "image/gif" },
	{ "png",     "image/png" },
	{ "jpg",     "image/jpeg" },
	{ "jpeg",    "image/jpeg" },
	{ "svg",     "image/svg+xml" },

	{ "zip",     "application/zip" },
	{ "pdf",     "application/pdf" },
	{ "xml",     "application/xml" },
	{ "xsl",     "application/xml" },
	{ "doc",     "application/msword" },
	{ "ppt",     "application/vnd.ms-powerpoint" },
	{ "xls",     "application/vnd.ms-excel" },
	{ "odt",     "application/vnd.oasis.opendocument.text" },
	{ "odp",     "application/vnd.oasis.opendocument.presentation" },
	{ "pl",      "application/x-perl" },
	{ "sh",      "application/x-shellscript" },
	{ "php",     "application/x-php" },
	{ "deb",     "application/x-deb" },
	{ "iso",     "application/x-cd-image" },
	{ "tar.gz",  "application/x-compressed-tar" },
	{ "tgz",     "application/x-compressed-tar" },
	{ "gz",      "application/x-gzip" },
	{ "tar.bz2", "application/x-bzip-compressed-tar" },
	{ "tbz",     "application/x-bzip-compressed-tar" },
	{ "bz2",     "application/x-bzip" },
	{ "tar",     "application/x-tar" },
	{ "rar",     "application/x-rar-compressed" },

	{ "mp3",     "audio/mpeg" },
	{ "ogg",     "audio/x-vorbis+ogg" },
	{ "wav",     "audio/x-wav" },

	{ "mpg",     "video/mpeg" },
	{ "mpeg",    "video/mpeg" },
	{ "avi",     "video/x-msvideo" },

	{ "README",  "text/plain" },
	{ "log",     "text/plain" },
	{ "cfg",     "text/plain" },
	{ "conf",    "text/plain" },

	{ "pac",		"application/x-ns-proxy-autoconfig" },
	{ "wpad.dat",	"application/x-ns-proxy-autoconfig" },

	{ NULL, NULL }
};

#endif

