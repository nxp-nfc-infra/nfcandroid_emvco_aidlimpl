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
#ifndef _PHOSALNFC_TML_H_
#define _PHOSALNFC_TML_H_

/**
 * Close.
 *
 * closes the file specified by pathname
 *
 * \param[in] pathname               path of the file.
 *
 * \retval #>0                       on success
 * \retval #-1                       error
 *
 */
int osal_tml_close(int fd);

/**
 * Open.
 *
 * opens the file specified by pathname
 *
 * \param[in] pathname                path of the file.
 *
 * \retval #0                         on success
 * \retval #-1                        error
 *
 */
int osal_tml_open(const char *pathname, int flags);

/**
 * Read.
 *
 * attempts to read up to count bytes from file descriptor fd
 * into the buffer starting at buf
 *
 * \param[in] fd                      file descriptor of the file.
 * \param[in] buf                     buffer to store the read data
 * \param[in] count                   read up to count bytes
 *
 * \retval #0 or >0                   on success
 * \retval #-1                        error
 *
 */
int osal_tml_read(int fd, void *buf, size_t count);

/**
 * Write.
 *
 * writes up to count bytes from the buffer starting at buf
 * to the file referred to by the file descriptor fd
 *
 * \param[in] fd                   file descriptor of the file.
 * \param[in] buf                  buffer to store the read data
 * \param[in] count                read up to count bytes
 *
 * \retval #0 or >0                on success
 * \retval #-1                     error
 *
 */
int osal_tml_write(int fd, const void *buf, size_t count);

/**
 * ioctl - Control device.
 *
 * performs a variety of control functions on STREAMS devices
 *
 * \param[in] fd                      file descriptor of the file
 * \param[in] request                 selects the control function
 *                                    to be performed
 * \param[in] reset_type              NFCC power control
 * \param[in] count                   read up to count bytes
 *
 * \retval #otherthan -1              on success
 * \retval #-1                        error
 *
 */
int osal_tml_ioctl(int fd, unsigned long request, unsigned long reset_type,
                   int count);

#endif /* _PHOSALNFC_TML_H_*/