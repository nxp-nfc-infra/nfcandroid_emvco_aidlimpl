/******************************************************************************
 *
 *  Copyright 2022 NXP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#include <fcntl.h>
#include <phNxpLog.h>
#include <sys/ioctl.h>
#include <unistd.h>

int phOsalNfc_Tml_close(int fd) {
  NXPLOG_TML_D("%s\n", __func__);
  return close(fd);
}

int phOsalNfc_Tml_open(const char *pathname, int flags) {
  NXPLOG_TML_D("%s Opening port=%s\n", __func__, pathname);
  return open(pathname, flags);
}

int phOsalNfc_Tml_read(int fd, void *buf, size_t count) {
  NXPLOG_TML_D("%s\n", __func__);
  return read(fd, buf, count);
}

int phOsalNfc_Tml_write(int fd, const void *buf, size_t count) {
  NXPLOG_TML_D("%s\n", __func__);
  return write(fd, buf, count);
}

int phOsalNfc_Tml_ioctl(int fd, unsigned long request, unsigned long reset_type,
                        int count) {
  NXPLOG_TML_D("%s\n", __func__);
  if (count == 1) {
    return ioctl(fd, request);
  } else if (count == 2) {
    return ioctl(fd, request, reset_type);
  } else {
    NXPLOG_TML_E(
        "%s\n  Failed. Not supported for more than one variable argument",
        __func__);
    return -1;
  }
}