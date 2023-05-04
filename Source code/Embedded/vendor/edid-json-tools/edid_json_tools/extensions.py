# Copyright 2014 The Chromium OS Authors. All rights reserved.
# Copyright (c) 2019-2021 The EDID JSON Tools authors. All rights reserved.
#
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# SPDX-License-Identifier: BSD-3-Clause


"""Provides Extension class and related classes with methods for parsing info.

Extensions are found after the 128-byte base EDID.
The various types of Extensions all inherit the basic Extension object.
"""

from typing import Final, List, Optional

from edid_json_tools.typing import ByteList, EdidVersion

from . import coordinated_video_timings as cvt_module
from . import data_block, descriptor, error, standard_timings

TYPE_TIMING_EXTENSION = "Timing Extension"
TYPE_CEA_861 = "CEA-861 Series Timing Extension"
TYPE_VIDEO_TIMING_BLOCK = "Video Timing Block Extension (VTB-EXT)"
TYPE_DISPLAY_INFORMATION = "Display Information Extension (DI-EXT)"
TYPE_LOCALIZED_STRING = "Localized String Extension (LS-EXT)"
TYPE_DIGITAL_PACKET_VIDEO_LINK = "Digital Packet Video Link Extension (DPVL-EXT)"
TYPE_EXTENSION_BLOCK_MAP = "Extension Block Map"
TYPE_MANUFACTURER_EXTENSION = "Extension defined by monitor manufacturer"
TYPE_UNKNOWN = "Unknown Extension type"


def GetExtension(edid: ByteList, index: int, version: EdidVersion):
    """Fetch an extension to an EDID.

    Args:
      edid: The list form of the EDID being analyzed.
      index: An integer indicating the index of the extension.
      version: The EDID version (usually 1.3 or 1.4).

    Returns:
      An Extension object.
    """
    block: ByteList = edid[(128 * index) : (128 * (index + 1))]

    tag = block[0]

    if tag == 0x00:
        return TimingExtension(block)
    elif tag == 0x02:
        return CEAExtension(block, version)
    elif tag == 0x10:
        return VTBExtension(block, version)
    elif tag == 0x40:
        return DisplayInformationExtension(block)
    elif tag == 0x50:
        return LocalizedStringExtension(block)
    elif tag == 0x60:
        return DPVLExtension(block)
    elif tag == 0xF0:
        return ExtensionBlockMap(block)
    elif tag == 0xFF:
        return ManufacturerExtension(block)
    else:
        return Extension(block, TYPE_UNKNOWN)


class Extension(object):
    """Defines a basic extension."""

    def __init__(
        self, block: ByteList, my_type: str, version: Optional[EdidVersion] = None
    ):
        """Create an Extension object.

        Args:
          block: A list of bytes that make up the extension.
          my_type: A string indicating the type of extension.
          version: The EDID version.
        """
        self._block: Final[ByteList] = block
        self._type: Final[str] = my_type
        self._version: Final[Optional[EdidVersion]] = version

    @property
    def tag(self) -> int:
        """Fetch the tag of the extension.

        Returns:
          An integer indicating the tag of the extension.
        """
        return self._block[0]

    @property
    def type(self) -> str:
        """Fetch the type of the extension.

        Returns:
          A string indicating the type of the extension.
        """
        return self._type

    def GetBlock(self) -> ByteList:
        """Fetch the bytes that make up the extension.

        Returns:
          A list of bytes that make up the extension.
        """
        return self._block

    def CheckErrors(self, index: Optional[int] = None) -> error.OptionalErrorList:
        """Create a method for error checking to define in other Extensions.

        Args:
          index: The integer index of the extension being checked.
        """


class TimingExtension(Extension):
    """Defines a Timing Extension."""

    def __init__(self, block: ByteList):
        """Create a TimingExtension object.

        Args:
          block: The list of bytes that make up the extension.
        """
        Extension.__init__(self, block, TYPE_TIMING_EXTENSION)


class CEAExtension(Extension):
    """Defines a CEA Extension, perhaps the most common type."""

    def __init__(self, block: ByteList, version):
        """Create a CEAExtension object.

        Args:
          block: The list of bytes that make up the extension.
          version: The version of the extension.
        """
        Extension.__init__(self, block, TYPE_CEA_861, version)
        self._dtd_start = block[2]

    @property
    def dtd_offset(self) -> int:
        """Fetch the start index of the Detailed Timing Descriptors.

        Returns:
          An integer indicating the start index of Detailed Timing Descriptors.
        """
        return self._dtd_start

    @property
    def version(self) -> int:
        """Fetch the extension version.

        Returns:
          An integer indicating the extension version.
        """
        return self._block[1]

    @property
    def underscan_support(self) -> bool:
        """Fetch whether underscan is supported.

        Returns:
          A boolean indicating whether underscan is supported.
        """
        return bool(self._block[3] & 0x80)

    @property
    def basic_audio_support(self) -> bool:
        """Fetch whether basic audio is supported.

        Returns:
          A boolean indicating whether basic audio is supported.
        """
        return bool(self._block[3] & 0x40)

    @property
    def ycbcr444_support(self) -> bool:
        """Fetch whether YCbCr 4:4:4 is supported.

        Returns:
          A boolean indicating whether YCbCr 4:4:4 is supported.
        """
        return bool(self._block[3] & 0x20)

    @property
    def ycbcr422_support(self) -> bool:
        """Fetch whether YCbCr 4:2:2 is supported.

        Returns:
          A boolean indicating whether YCbCr 4:2:2 is supported.
        """
        return bool(self._block[3] & 0x10)

    @property
    def native_dtd_count(self) -> int:
        """Fetch the number of native Detailed Timing Descriptors.

        Returns:
          An integer indicating the number of native Detailed Timing Descriptors.
        """
        return self._block[3] & 0x0F

    @property
    def data_blocks(self) -> List[data_block.DataBlock]:
        """Fetch the Data Block objects.

        Returns:
          A list of Data Block objects.
        """
        # DTDs begin immediately, meaning there are no data blocks
        if self._dtd_start == 4:
            return []

        dbs = []

        current = 4

        while current < self._dtd_start:
            db = data_block.GetDataBlock(self._block, current)
            dbs.append(db)

            current += db.length + 1

        return dbs

    @property
    def dtds(self) -> List[descriptor.DetailedTimingDescriptor]:
        """Fetch the descriptor.DetailedTimingDescriptor objects.

        Returns:
          A list of descriptor.DetailedTimingDescriptor objects.
        """
        dtds = []

        for x in range(self._dtd_start, self._GetPadIndex(), 18):
            dtd = descriptor.GetDescriptor(self._block, x, self._version)
            assert isinstance(dtd, descriptor.DetailedTimingDescriptor)

            dtds.append(dtd)

        return dtds

    def _GetPadIndex(self) -> int:
        """Fetch the start index of post-DTD padding.

        Returns:
          An integer indicating the start index of post-DTD padding.
        """
        for x in range(self._dtd_start, 127, 18):
            if self._block[x] == self._block[x + 1] == 0:
                return x
        return 127 - (127 - self._dtd_start) % 18

    def CheckErrors(self, index: Optional[int] = None) -> error.OptionalErrorList:
        """Check the extension for errors.

        All bytes after the end of DTDs should be 0x00.

        Args:
          index: The integer index of the extension.

        Returns:
          An Error object.
        """
        ext_index = "(Extension #%d)" % index if index else ""
        padding_start = self._GetPadIndex()
        padding = self._block[padding_start:127]

        if padding != [0] * len(padding):
            found = (
                "%02X "
                * len(padding)
                % tuple(
                    padding,
                )
            )
            return [
                error.Error(
                    "CEA extension %s" % ext_index,
                    "All bytes after DTDs should be 0x00",
                    "All 0x00s",
                    found,
                )
            ]
        else:
            return None


class VTBExtension(Extension):
    """Defines a VTB Extension."""

    def __init__(self, block: ByteList, version: EdidVersion):
        """Create a VTBExtension object.

        Args:
          block: The list of bytes that make up the extension.
          version: The version of the extension.
        """
        Extension.__init__(self, block, TYPE_VIDEO_TIMING_BLOCK, version)
        self._dtb_count: Final[int] = block[2]
        self._cvt_count: Final[int] = block[3]
        self._st_count: Final[int] = block[4]

    @property
    def version(self) -> int:
        """Fetch the VTB extension version.

        Returns:
          An integer indicating the version.
        """
        return self._block[1]

    @property
    def dtb_count(self) -> int:
        """Fetch the number of DetailedTimingDescriptor objects.

        Returns:
          An integer indicating the number of DetailedTimingDescriptor objects.
        """
        return self._dtb_count

    @property
    def cvt_count(self) -> int:
        """Fetch the number of CoordinatedVideoTiming objects.

        Returns:
          An integer indicating the number of CoordinatedVideoTiming objects.
        """
        return self._cvt_count

    @property
    def st_count(self) -> int:
        """Fetch the number of StandardTiming objects.

        Returns:
          An integer indicating the number of StandardTiming objects.
        """
        return self._st_count

    @property
    def dtbs(self) -> List[descriptor.DetailedTimingDescriptor]:
        """Fetch the descriptor.DetailedTimingDescriptors.

        Returns:
          A list of descriptor.DetailedTimingDescriptors.
        """
        dtbs = []
        dtb_start = 5

        for x in range(0, self._dtb_count):
            dtd = descriptor.GetDescriptor(
                self._block, dtb_start + (x * 18), self._version
            )
            assert isinstance(dtd, descriptor.DetailedTimingDescriptor)
            dtbs.append(dtd)

        return dtbs

    @property
    def cvts(self):
        """Fetch the coordinated_video_timings.CoordinatedVideoTiming objects.

        Returns:
          A list of coordinated_video_timings.CoordinatedVideoTiming objects.
        """
        cvts = []
        cvt_start = 5 + (18 * self._dtb_count)

        for x in range(0, self._cvt_count):
            cvt = cvt_module.GetCoordinatedVideoTiming(self._block, cvt_start + (x * 3))
            cvts.append(cvt)

        return cvts

    @property
    def sts(self):
        """Fetch the standard_timings.StandardTiming objects.

        Returns:
          A list of standard_timings.StandardTiming objects.
        """
        sts = []
        st_start = 5 + (18 * self._dtb_count) + (3 * self._cvt_count)

        for x in range(0, self._st_count):
            st = standard_timings.GetStandardTiming(
                self._block, st_start + (x * 2), self._version
            )
            sts.append(st)

        return sts

    def CheckErrors(self, index: Optional[int] = None) -> error.ErrorList:
        """Check the extension for errors.

        Args:
          index: The integer index of the extension.

        Returns:
          A list of error.Error objects.
        """
        errors = []

        cvts = self.cvts
        for i, cvt in enumerate(cvts, 1):
            err = cvt.CheckErrors(i)
            if err:
                errors.extend(err)

        sts = self.sts
        for i, st in enumerate(sts, 1):
            err = st.CheckErrors(i)
            if err:
                errors.extend(err)

        # Check that all bytes after standard timing blocks end are 0x00
        unused_start = 5
        unused_start += 18 * self._dtb_count
        unused_start += 3 * self._cvt_count
        unused_start += 2 * self._st_count

        ext_index = "(Extension #%d)" % index if index else ""
        padding = self._block[unused_start:127]
        if padding != [0] * len(padding):
            found = (
                "0x%02X "
                * len(padding)
                % tuple(
                    padding,
                )
            )
            errors.append(
                error.Error(
                    "VTB Extension %s" % ext_index,
                    "All bytes after STs should be 0x00",
                    "All 0x00s",
                    found,
                )
            )
        return errors


class DisplayInformationExtension(Extension):
    """Analyzes a Display Information Extension."""

    def __init__(self, block: ByteList):
        """Create a DisplayInformationExtension object.

        Args:
          block: The list of bytes that make up the extension.
        """
        Extension.__init__(self, block, TYPE_DISPLAY_INFORMATION)


class LocalizedStringExtension(Extension):
    """Analyzes a Localized String Extension."""

    def __init__(self, block: ByteList):
        """Create a LocalizedStringExtension object.

        Args:
          block: The list of bytes that make up the extension.
        """
        Extension.__init__(self, block, TYPE_LOCALIZED_STRING)


class DPVLExtension(Extension):
    """Analyzes a Digital Packet Video Link Extension."""

    def __init__(self, block: ByteList):
        """Create a DPVLExtension object.

        Args:
          block: The list of bytes that make up the extension.
        """
        Extension.__init__(self, block, TYPE_DIGITAL_PACKET_VIDEO_LINK)


class ExtensionBlockMap(Extension):
    """Defines an Extension Block Map (which is also an extension)."""

    def __init__(self, block: ByteList):
        """Create an ExtensionBlockMap object.

        Args:
          block: The list of bytes that make up the extension.
        """
        Extension.__init__(self, block, TYPE_EXTENSION_BLOCK_MAP)

    @property
    def all_tags(self):
        """Fetch all 126 tags in the block map.

        Zeroes indicate unused blocks.

        Returns:
          A list of 126 tags (integers) indicating the tag of each following block.
        """
        return self._block[1:127]


class ManufacturerExtension(Extension):
    """Defines a Manufacturer Extension."""

    def __init__(self, block: ByteList):
        """Create a ManufacturerExtension object.

        Args:
          block: The list of bytes that make up the extension.
        """
        Extension.__init__(self, block, TYPE_MANUFACTURER_EXTENSION)
