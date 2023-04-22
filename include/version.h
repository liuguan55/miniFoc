//
// Created by 邓志君 on 2023/4/16.
//

#ifndef MINIFOC_F4_INCLUDE_VERSION_H_
#define MINIFOC_F4_INCLUDE_VERSION_H_

#define SDK_VERSION_MAJOR 1
#define SDK_VERSION_MINOR 2
#define SDK_VERSION_PATCH 0

#define SDK_VERSION_NUM ((SDK_VERSION_MAJOR << 16) | \
                         (SDK_VERSION_MINOR << 8)  | \
                         (SDK_VERSION_PATCH))

#define __SDK_VERSTR(x)	#x
#define _SDK_VERSTR(x)	__SDK_VERSTR(x)
#define SDK_VERSION_STR _SDK_VERSTR(SDK_VERSION_MAJOR) "." \
                        _SDK_VERSTR(SDK_VERSION_MINOR) "." \
                        _SDK_VERSTR(SDK_VERSION_PATCH)

#define SDK_STAGE_STR ""

#endif //MINIFOC_F4_INCLUDE_VERSION_H_
