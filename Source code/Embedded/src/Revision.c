/** @file
    @brief Implementation

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// All rights reserved.

// Internal Includes
#include "Revision.h"

// Library/third-party includes
// - none

// Standard includes
// - none

#if defined(SVR_HMDMCU_MAKEFILE)
#include "gitdefines.h"
#if SVR_GIT_NOT_FOUND
#define SVR_IS_DEV_BUILD
#define SVR_REVISION_UNKNOWN
#endif // SVR_GIT_NOT_FOUND

#else
// IDE builds
#define SVR_REVISION_UNKNOWN
#endif

#ifdef SVR_HMDMCU_MAKEFILE

#if SVR_GIT_DIRTY || SVR_GIT_NOT_FOUND || !SVR_GIT_AT_TAG
#define HAVE_SUFFIX
const char * svr_version_suffix = " (DEV)";
#endif

#else // not makefile
#define SVR_IS_IDE_DEV_BUILD
#define HAVE_SUFFIX
const char * svr_version_suffix = " (DEV-IDE)";
#endif

#ifndef HAVE_SUFFIX
const char * svr_version_suffix = " (RELEASE)";
#endif

const char * svr_hmdmcu_revision =
#if defined(SVR_IS_DEV_BUILD)
    "(DEV) "
#elif defined(SVR_IS_IDE_DEV_BUILD)
    "(DEV-IDE) "
#endif

#ifdef SVR_HMDMCU_MAKEFILE
#if SVR_GIT_AT_TAG
    "Built from source tagged " SVR_GIT_LAST_TAG
#else // not SVR_GIT_AT_TAG
    "Based on source tagged " SVR_GIT_LAST_TAG " and " SVR_GIT_COMMIT_COUNT " additional commits (commit hash " SVR_GIT_COMMIT_HASH ")"
#endif // SVR_GIT_AT_TAG
#if SVR_GIT_DIRTY
    " with additional (uncommitted) changes"
#endif // SVR_GIT_DIRTY
#endif // SVR_HMDMCU_MAKEFILE

#ifdef SVR_REVISION_UNKNOWN
    "Dev build with no Git revision information available"
#endif
;

#ifdef SVR_HMDMCU_BUILDINFO
const char * svr_hmdmcu_extrabuildinfo = SVR_HMDMCU_BUILDINFO;
#else
const char * svr_hmdmcu_extrabuildinfo = "";
#endif
