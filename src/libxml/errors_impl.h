/*
 * Copyright (C) 2013 Vaclav Slavik <vslavik@gmail.com>
 * All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of the Author nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _xmlwrapp_errors_impl_h_
#define _xmlwrapp_errors_impl_h_

#include <xmlwrapp/errors.h>

namespace xml
{

namespace impl
{

// This handler collects all error & warning messages from libxml2 callbacks,
// without throwing any exceptions, and then replays them, in order, to the
// "real" error handler.
class XMLWRAPP_API errors_collector : public error_messages
{
public:
    // replay all errors into target handler
    void replay(error_handler& dest);

protected:
    virtual std::string format_for_print(const error_message& msg) const;
};

// These functions can be used as error callbacks in various libxml2 functions.
// They collect messages into errors_collector structure.
// Usage: pass the pointer to errors_collector as libxml2's void* ctx argument
extern "C"
{

XMLWRAPP_API void cb_messages_warning(void *out, const char *message, ...);
XMLWRAPP_API void cb_messages_error(void *out, const char *message, ...);

XMLWRAPP_API void cb_messages_warning_v(void *out, const char *message, va_list ap);
XMLWRAPP_API void cb_messages_error_v(void *out, const char *message, va_list ap);

#define CALL_CB_MESSAGES(cb, out, message)        \
    va_list ap;                                   \
    va_start(ap, message);                        \
    cb(out, message, ap);                         \
    va_end(ap)

#define CALL_CB_MESSAGES_ERROR(out, message) \
    CALL_CB_MESSAGES(cb_messages_error_v, out, message)

#define CALL_CB_MESSAGES_WARNING(out, message) \
    CALL_CB_MESSAGES(cb_messages_warning_v, out, message)

} // extern "C"

} // namespace impl

} // namespace xml


#endif // _xmlwrapp_errors_impl_h_