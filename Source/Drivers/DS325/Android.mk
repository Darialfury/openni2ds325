# OpenNI 2.x Android makefile. 
# Copyright (C) 2012 PrimeSense Ltd.
# Copyright (C) 2014 SIProp.org http://www.siprop.org/
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License. 

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# Sources
MY_SRC_FILES := \
	$(LOCAL_PATH)/DriverImpl/*.cpp
	
MY_SRC_FILE_EXPANDED := $(wildcard $(MY_SRC_FILES))
LOCAL_SRC_FILES := $(MY_SRC_FILE_EXPANDED:$(LOCAL_PATH)/%=%)

# C/CPP Flags
LOCAL_CFLAGS += $(OPENNI2_CFLAGS)
LOCAL_CPPFLAGS := -frtti -fpermissive

# Includes
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/. \
	$(LOCAL_PATH)/Include \
	$(LOCAL_PATH)/../../DepthUtils \
	$(LOCAL_PATH)/../../../Include \
	$(LOCAL_PATH)/../../../ThirdParty/PSCommon/XnLib/Include

# Dependencies	
LOCAL_STATIC_LIBRARIES := XnLib DepthUtils
LOCAL_SHARED_LIBRARIES := liblog libdl libgabi++
LOCAL_LDLIBS += -llog

# Output
LOCAL_MODULE:= libDS325

include $(BUILD_SHARED_LIBRARY)
