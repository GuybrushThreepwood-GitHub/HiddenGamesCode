
# CURL NDK makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := curl

ZLIB_FOLDER := $(call host-path, $(SDK_SRC_ROOT)/zlib-1.2.8)
ZLIB_INCLUDE := $(SDK_INC_ROOT)/zlib-1.2.8/include

HOST=$(shell hostname)

ifeq ($(HOST),mips)
CURL_FOLDER := $(call host-path, $(SDK_SRC_ROOT)/curl-7.34.0)
CURL_INCLUDE := $(SDK_INC_ROOT)/curl-7.34.0/include $(SDK_INC_ROOT)/curl-7.34.0/build/ndk/mips $(ZLIB_INCLUDE)
else
CURL_FOLDER := $(call host-path, $(SDK_SRC_ROOT)/curl-7.34.0)
CURL_INCLUDE := $(SDK_INC_ROOT)/curl-7.34.0/include $(SDK_INC_ROOT)/curl-7.34.0/build/ndk $(ZLIB_INCLUDE)
endif

CURL_SRC := $(CURL_FOLDER)/lib/amigaos.c \
$(CURL_FOLDER)/lib/asyn-ares.c \
$(CURL_FOLDER)/lib/asyn-thread.c \
$(CURL_FOLDER)/lib/axtls.c \
$(CURL_FOLDER)/lib/base64.c \
$(CURL_FOLDER)/lib/bundles.c \
$(CURL_FOLDER)/lib/conncache.c \
$(CURL_FOLDER)/lib/connect.c \
$(CURL_FOLDER)/lib/content_encoding.c \
$(CURL_FOLDER)/lib/cookie.c \
$(CURL_FOLDER)/lib/curl_addrinfo.c \
$(CURL_FOLDER)/lib/curl_darwinssl.c \
$(CURL_FOLDER)/lib/curl_fnmatch.c \
$(CURL_FOLDER)/lib/curl_gethostname.c \
$(CURL_FOLDER)/lib/curl_gssapi.c \
$(CURL_FOLDER)/lib/curl_memrchr.c \
$(CURL_FOLDER)/lib/curl_multibyte.c \
$(CURL_FOLDER)/lib/curl_ntlm.c \
$(CURL_FOLDER)/lib/curl_ntlm_core.c \
$(CURL_FOLDER)/lib/curl_ntlm_msgs.c \
$(CURL_FOLDER)/lib/curl_ntlm_wb.c \
$(CURL_FOLDER)/lib/curl_rtmp.c \
$(CURL_FOLDER)/lib/curl_sasl.c \
$(CURL_FOLDER)/lib/curl_schannel.c \
$(CURL_FOLDER)/lib/curl_sspi.c \
$(CURL_FOLDER)/lib/curl_threads.c \
$(CURL_FOLDER)/lib/cyassl.c \
$(CURL_FOLDER)/lib/dict.c \
$(CURL_FOLDER)/lib/dotdot.c \
$(CURL_FOLDER)/lib/easy.c \
$(CURL_FOLDER)/lib/escape.c \
$(CURL_FOLDER)/lib/file.c \
$(CURL_FOLDER)/lib/fileinfo.c \
$(CURL_FOLDER)/lib/formdata.c \
$(CURL_FOLDER)/lib/ftp.c \
$(CURL_FOLDER)/lib/ftplistparser.c \
$(CURL_FOLDER)/lib/getenv.c \
$(CURL_FOLDER)/lib/getinfo.c \
$(CURL_FOLDER)/lib/gopher.c \
$(CURL_FOLDER)/lib/gskit.c \
$(CURL_FOLDER)/lib/gtls.c \
$(CURL_FOLDER)/lib/hash.c \
$(CURL_FOLDER)/lib/hmac.c \
$(CURL_FOLDER)/lib/hostasyn.c \
$(CURL_FOLDER)/lib/hostcheck.c \
$(CURL_FOLDER)/lib/hostip.c \
$(CURL_FOLDER)/lib/hostip4.c \
$(CURL_FOLDER)/lib/hostip6.c \
$(CURL_FOLDER)/lib/hostsyn.c \
$(CURL_FOLDER)/lib/http.c \
$(CURL_FOLDER)/lib/http_chunks.c \
$(CURL_FOLDER)/lib/http_digest.c \
$(CURL_FOLDER)/lib/http_negotiate.c \
$(CURL_FOLDER)/lib/http_negotiate_sspi.c \
$(CURL_FOLDER)/lib/http_proxy.c \
$(CURL_FOLDER)/lib/http2.c \
$(CURL_FOLDER)/lib/idn_win32.c \
$(CURL_FOLDER)/lib/if2ip.c \
$(CURL_FOLDER)/lib/imap.c \
$(CURL_FOLDER)/lib/inet_ntop.c \
$(CURL_FOLDER)/lib/inet_pton.c \
$(CURL_FOLDER)/lib/krb5.c \
$(CURL_FOLDER)/lib/ldap.c \
$(CURL_FOLDER)/lib/llist.c \
$(CURL_FOLDER)/lib/md4.c \
$(CURL_FOLDER)/lib/md5.c \
$(CURL_FOLDER)/lib/memdebug.c \
$(CURL_FOLDER)/lib/mprintf.c \
$(CURL_FOLDER)/lib/multi.c \
$(CURL_FOLDER)/lib/netrc.c \
$(CURL_FOLDER)/lib/non-ascii.c \
$(CURL_FOLDER)/lib/nonblock.c \
$(CURL_FOLDER)/lib/nss.c \
$(CURL_FOLDER)/lib/nwlib.c \
$(CURL_FOLDER)/lib/nwos.c \
$(CURL_FOLDER)/lib/openldap.c \
$(CURL_FOLDER)/lib/parsedate.c \
$(CURL_FOLDER)/lib/pingpong.c \
$(CURL_FOLDER)/lib/pipeline.c \
$(CURL_FOLDER)/lib/polarssl.c \
$(CURL_FOLDER)/lib/polarssl_threadlock.c \
$(CURL_FOLDER)/lib/pop3.c \
$(CURL_FOLDER)/lib/progress.c \
$(CURL_FOLDER)/lib/qssl.c \
$(CURL_FOLDER)/lib/rawstr.c \
$(CURL_FOLDER)/lib/rtsp.c \
$(CURL_FOLDER)/lib/security.c \
$(CURL_FOLDER)/lib/select.c \
$(CURL_FOLDER)/lib/sendf.c \
$(CURL_FOLDER)/lib/share.c \
$(CURL_FOLDER)/lib/slist.c \
$(CURL_FOLDER)/lib/smtp.c \
$(CURL_FOLDER)/lib/socks.c \
$(CURL_FOLDER)/lib/socks_gssapi.c \
$(CURL_FOLDER)/lib/socks_sspi.c \
$(CURL_FOLDER)/lib/speedcheck.c \
$(CURL_FOLDER)/lib/splay.c \
$(CURL_FOLDER)/lib/ssh.c \
$(CURL_FOLDER)/lib/sslgen.c \
$(CURL_FOLDER)/lib/ssluse.c \
$(CURL_FOLDER)/lib/strdup.c \
$(CURL_FOLDER)/lib/strequal.c \
$(CURL_FOLDER)/lib/strerror.c \
$(CURL_FOLDER)/lib/strtok.c \
$(CURL_FOLDER)/lib/strtoofft.c \
$(CURL_FOLDER)/lib/telnet.c \
$(CURL_FOLDER)/lib/tftp.c \
$(CURL_FOLDER)/lib/timeval.c \
$(CURL_FOLDER)/lib/transfer.c \
$(CURL_FOLDER)/lib/url.c \
$(CURL_FOLDER)/lib/version.c \
$(CURL_FOLDER)/lib/warnless.c \
$(CURL_FOLDER)/lib/wildcard.c \
$(CURL_FOLDER)/lib/x509asn1.c


LOCAL_C_INCLUDES := $(CURL_INCLUDE)
LOCAL_SRC_FILES :=  $(CURL_SRC)

LOCAL_CFLAGS 	:= -DHAVE_CONFIG_H -DBUILDING_LIBCURL -DCURL_STATICLIB -ffast-math -O3 -funroll-loops

include $(BUILD_STATIC_LIBRARY)
