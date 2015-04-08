const char cInput[] PROGMEM="Input ";
const char cA[] PROGMEM="A: ";
const char cB[] PROGMEM="B: ";
const char cC[] PROGMEM="C: ";
const char cD[] PROGMEM="D: ";
const char cUnknown[] PROGMEM="Unknown: ";
const char cDigitalActivityLost[] PROGMEM="Digital ACTIVITY LOST";
const char cDigitalActivityDetected[] PROGMEM="Digital ACTIVITY DETECTED";
const char SwitchProblem[] PROGMEM="Switch problem";
const char cReceiverA[] PROGMEM="Receiver A:";
const char cReceiverB[] PROGMEM="Receiver B:";
const char cInfoframe[] PROGMEM="Info frame: ";
const char cISRC1Received[] PROGMEM="ISRC1 received";
const char cISRC2Received[] PROGMEM="ISRC2 received";
const char cACPReceived[] PROGMEM="ACP Received";
const char cAVIReceived[] PROGMEM="AVI Received";
const char cWrongPacketType[] PROGMEM="AVI: wrong packet type!";
const char cColorSpace[] PROGMEM="Color space=";
const char cRGB[] PROGMEM="RGB";
const char cYCbCr422[] PROGMEM="YCbCr 422";
const char cYCbCr444[] PROGMEM="YCbCr 444";
const char cFuture[] PROGMEM="Future";
const char cWrongColorIndicator[] PROGMEM="Wrong color indicator";
const char cNoData[] PROGMEM="colorimetry:No data";
const char cSMPTE170M[] PROGMEM="colorimetry:SMPTE 170M / ITU601";
const char cITU709[] PROGMEM="colorimetry:ITU709";
const char cExtendedxvYCC[] PROGMEM="colorimetry:extended (xvYCC)";
const char cWrong[] PROGMEM="Wrong colorimetry";
const char cInfoframeSPDreceived[] PROGMEM="INFOFRAME: SPD received";
const char cInfoframeAUDreceived[] PROGMEM="INFOFRAME: AUD received";
const char cInfoframeMPEGReceived[] PROGMEM="INFOFRAME: MPEG received";
const char cGamutMetadataReceived[] PROGMEM="Gamut metadata packets received";
const char cErroneousEventInfoframe[] PROGMEM="erroneous event for infoframe callback";
const char cAutomaticALocked[] PROGMEM="Automatic INPUT A LOCKED";
const char cAutomaticBLocked[] PROGMEM="Automatic INPUT B LOCKED";
const char cAutomaticDetectionOfResolution[] PROGMEM="Automatic detection of resolution: ";
const char cHDMIMode[] PROGMEM="HDMI mode";
const char cDVIMode[] PROGMEM="DVI mode";
const char cHDCPEncrypted[] PROGMEM="HDCP encrypted ";
const char cNotHDCPEncrypted[] PROGMEM="not HDCP encrypted ";
const char cEESSMode[] PROGMEM="EESS mode";
const char cDVIOESSMode[] PROGMEM="DVI (OESS) mode";
const char cAVMuteActive[] PROGMEM="AV mute active";
const char cAVMuteInactive[] PROGMEM="AV Mute inactive";
const char cDeepColor24Bit[] PROGMEM="Deep color mode: 24 bits";
const char cDeepColor30Bit[] PROGMEM="Deep color mode: 30 bits";
const char cDeepColor36Bit[] PROGMEM="Deep color mode: 36 bits";
const char cAudioSamplePackets[] PROGMEM="Audio sample packets";
const char cAudioHBRPackets[] PROGMEM="Audio HBR packets";
const char cAudioOBAPackets[] PROGMEM="Audio OBA packets";
const char cAudioDSTPackets[] PROGMEM="Audio DST packets";
const char cCalculationError[] PROGMEM="calculation error";
const char cAudioSample32KHz[] PROGMEM="Audio frequency: 32 kHz";
const char cAudioSample44KHz[] PROGMEM="Audio frequency: : 44.1 kHz";
const char cAudioSample48KHz[] PROGMEM="Audio frequency: : 48 kHz";
const char cAudioSample88KHz[] PROGMEM="Audio frequency: : 88.2 kHz";
const char cAudioSample96KHz[] PROGMEM="Audio frequency: : 96 kHz";
const char cAudioSample176KHz[] PROGMEM="Audio frequency: : 176.4 kHz";
const char cAudioSample192KHz[] PROGMEM="Audio frequency: : 192 kHz";
const char cWrongValue[] PROGMEM=": wrong value";
const char cRepeaterState5Reached[] PROGMEM="Repeater state 5 reached";
const char cErroneousEvent[] PROGMEM="erroneous event for standard event callback";
const char c720_480p_60HZ[] PROGMEM="720x480p59/60Hz";
const char c1280_720p_60HZ[] PROGMEM="1280x720p59/60Hz";
const char c1920_1080i_60HZ[] PROGMEM="1920x1080i59/60Hz";
const char c720_480i_60HZ[] PROGMEM="720x480i59/60Hz";
const char c1920_1080p_60HZ[] PROGMEM="1920x1080p59/60Hz";
const char c1920_1080p_30HZ[] PROGMEM="1920x1080p29/30Hz";
const char c1080_1920p_60HZ[] PROGMEM="1080x1920p/60Hz";
const char c640_480p_60HZ[] PROGMEM="VGA 640x480p/60Hz";
const char c800_600p_60HZ[] PROGMEM="SVGA 800x600p/60Hz";
const char c1024_768p_60HZ[] PROGMEM="XGA 1024x768p/60Hz";
const char c1280_768p_60HZ[] PROGMEM="WXGA 1280x768p/60Hz";
const char c1280_960p_60HZ[] PROGMEM="1280x960p/60Hz";
const char c1280_1024p_60HZ[] PROGMEM="SXGA 1280x1024p/60Hz";
const char c1600_1024p_60HZ[] PROGMEM="1600x1024p/60Hz";
const char c1600_1200p_60HZ[] PROGMEM="UGA 1600x1200p/60Hz";
const char c1680_1050p_60HZ_RB[] PROGMEM="WSXA 1680x1050p/60Hz";
const char c1920_1200p_60HZ_RB[] PROGMEM="WUXGA 1920x1200p/60Hz";
const char c1360_768p_60HZ[] PROGMEM="1360x768p/60Hz";
const char c1400_1050p_60HZ[] PROGMEM="1400x1050p/60Hz";
const char c1400_1050p_60HZ_RB[] PROGMEM="1400x1050p/60Hz reduced blanking";
const char cUnknownResolution[] PROGMEM="Unknown resolution";
const char cRGBColorSpace[] PROGMEM="RGB color space";
const char cPCFormatRGBFullRange[] PROGMEM="PC format => RGB full range";
const char cTVFormatRGBLimitedRange[] PROGMEM="TV format => RGB limited range";
const char cYCBCRColorSpace[] PROGMEM="YCBCR color space";
const char cNoColorimetryData[] PROGMEM="No colorimetry data";
const char cXVYCCColorimetryData[] PROGMEM="XVYCC colorimetry data";
