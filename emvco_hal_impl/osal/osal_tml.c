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

#include <emvco_log.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

int osal_tml_close(int fd) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return close(fd);
}

int osal_tml_open(const char *pathname, int flags) {
  LOG_EMVCO_TML_D("%s Opening port=%s\n", __func__, pathname);
  return open(pathname, flags);
}

int osal_tml_read(int fd, void *buf, size_t count) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return read(fd, buf, count);
}

int osal_tml_write(int fd, const void *buf, size_t count) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return write(fd, buf, count);
}

int osal_tml_ioctl(int fd, unsigned long request, unsigned long reset_type,
                   int count) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  if (count == 1) {
    return ioctl(fd, request);
  } else if (count == 2) {
    return ioctl(fd, request, reset_type);
  } else {
    LOG_EMVCO_TML_E(
        "%s\n  Failed. Not supported for more than one variable argument",
        __func__);
    return -1;
  }
}