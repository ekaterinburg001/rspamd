*** Settings ***
Library         ${RSPAMD_TESTDIR}/lib/rspamd.py
Resource        ${RSPAMD_TESTDIR}/lib/rspamd.robot
Variables       ${RSPAMD_TESTDIR}/lib/vars.py

*** Variables ***
${MESSAGE}         ${RSPAMD_TESTDIR}/messages/spam_message.eml

*** Test Cases ***
RBL FROM MISS
  Scan File  ${MESSAGE}  IP=1.2.3.4
  ...  Settings={symbols_enabled = [FAKE_RBL_UNKNOWN_CHECK]}
  Do Not Expect Symbol  FAKE_RBL_CODE_2

RBL FROM HIT
  Scan File  ${MESSAGE}  IP=4.3.2.1
  ...  Settings={symbols_enabled = [FAKE_RBL_UNKNOWN_CHECK]}
  Expect Symbol  FAKE_RBL_CODE_2

RBL FROM MULTIPLE HIT
  Scan File  ${MESSAGE}  IP=4.3.2.3
  ...  Settings={symbols_enabled = [FAKE_RBL_UNKNOWN_CHECK]}
  Expect Symbol  FAKE_RBL_CODE_2
  Expect Symbol  FAKE_RBL_CODE_3

RBL FROM UNKNOWN HIT
  Scan File  ${MESSAGE}  IP=4.3.2.2
  ...  Settings={symbols_enabled = [FAKE_RBL_UNKNOWN_CHECK]}
  Expect Symbol  FAKE_RBL_FAKE_RBL_UNKNOWN

RBL RECEIVED HIT
  Scan File  ${MESSAGE}  IP=8.8.8.8
  ...  Settings={symbols_enabled = [FAKE_RECEIVED_RBL_FAKE_RBL_UNKNOWN]}
  Expect Symbol  FAKE_RECEIVED_RBL_CODE_3

RBL FROM HIT WL
  Scan File  ${MESSAGE}  IP=4.3.2.4
  ...  Settings={symbols_enabled = [FAKE_RBL_UNKNOWN, FAKE_WL_RBL_UNKNOWN]}
  Do Not Expect Symbol  FAKE_RBL_CODE_2
  Expect Symbol With Exact Options  FAKE_WL_RBL_CODE_2  4.3.2.4:from

EMAILBL Compose Map 1
  Scan File  ${RSPAMD_TESTDIR}/messages/url14.eml
  ...  Settings={symbols_enabled = [RSPAMD_EMAILBL]}
  Expect Symbol With Exact Options  RSPAMD_EMAILBL  dirty.sanchez.com:email

EMAILBL Compose Map 2
  Scan File  ${RSPAMD_TESTDIR}/messages/url15.eml
  ...  Settings={symbols_enabled = [RSPAMD_EMAILBL]}
  Expect Symbol With Exact Options  RSPAMD_EMAILBL  very.dirty.sanchez.com:email

EMAILBL Compose Map 3
  Scan File  ${RSPAMD_TESTDIR}/messages/url16.eml
  ...  Settings={symbols_enabled = [RSPAMD_EMAILBL]}
  Expect Symbol With Exact Options  RSPAMD_EMAILBL  41.black.sanchez.com:email

CONTENT URLS
  Scan File  ${RSPAMD_TESTDIR}/messages/content_url.eml
  ...  Settings={symbols_enabled = [URIBL_CONTENTONLY, URIBL_NOCONTENT, URIBL_WITHCONTENT]}
  Expect Symbol With Exact Options  URIBL_NOCONTENT  example.org:url
  Expect Symbol With Option  URIBL_WITHCONTENT  example.com:url
  Expect Symbol With Option  URIBL_WITHCONTENT  example.org:url
  Expect Symbol With Option  URIBL_WITHCONTENT  8.8.8.8:url
  Expect Symbol With Exact Options  URIBL_CONTENTONLY  example.com:url

SELECTORS
  Scan File  ${RSPAMD_TESTDIR}/messages/btc.eml  From=user@example.com  Helo=example.org
  ...  Settings={symbols_enabled = [RBL_SELECTOR_SINGLE, RBL_SELECTOR_MULTIPLE]}
  Expect Symbol With Exact Options  RBL_SELECTOR_SINGLE  example.org:selector
  Expect Symbol With Option  RBL_SELECTOR_MULTIPLE  example.com:sel_from
  Expect Symbol With Option  RBL_SELECTOR_MULTIPLE  example.org:sel_helo

SELECTORS COMBINED
  Scan File  ${RSPAMD_TESTDIR}/messages/btc.eml  From=user@example.org  Helo=example.org
  ...  Settings={symbols_enabled = [RBL_SELECTOR_MULTIPLE]}
  Expect Symbol With Option  RBL_SELECTOR_MULTIPLE  example.org:sel_from
  Expect Symbol With Option  RBL_SELECTOR_MULTIPLE  example.org:sel_helo

NUMERIC URLS
  Scan File  ${RSPAMD_TESTDIR}/messages/numeric_urls.eml
  ...  Settings={symbols_enabled = [URIBL_NUMERIC]}
  Expect Symbol With Exact Options  URIBL_NUMERIC  4.3.2.1:url

NUMERIC URLS WITH IMAGES
  Scan File  ${RSPAMD_TESTDIR}/messages/numeric_urls.eml
  ...  Settings={symbols_enabled = [URIBL_NUMERIC_IMAGES]}
  Expect Symbol With Exact Options  URIBL_NUMERIC_IMAGES  4.3.2.1:url  12.11.10.9:url

NUMERIC URLS WITH CONTENT
  Scan File  ${RSPAMD_TESTDIR}/messages/numeric_urls.eml
  ...  Settings={symbols_enabled = [URIBL_NUMERIC_CONTENT]}
  Expect Symbol With Exact Options  URIBL_NUMERIC_CONTENT  4.3.2.1:url  8.7.6.5:url

NUMERIC URLS WITH EVERYTHING
  Scan File  ${RSPAMD_TESTDIR}/messages/numeric_urls.eml
  ...  IP=127.0.0.1
  ...  Settings={symbols_enabled = [URIBL_NUMERIC_EVERYTHING]}
  Expect Symbol With Exact Options  URIBL_NUMERIC_EVERYTHING  12.11.10.9:url  4.3.2.1:url  8.7.6.5:url

NONNUMERIC URLS VANILLA
  Scan File  ${RSPAMD_TESTDIR}/messages/numeric_urls.eml
  ...  Settings={symbols_enabled = [URIBL_NONNUMERIC_VANILLA]}
  # Content
  Do Not Expect Symbol With Option  URIBL_NONNUMERIC_VANILLA  example.com:url
  # Image
  Do Not Expect Symbol With Option  URIBL_NONNUMERIC_VANILLA  judo.za.org:url
  # URL
  Expect Symbol With Option  URIBL_NONNUMERIC_VANILLA  example.org:url
  # Numeric
  Do Not Expect Symbol With Option  URIBL_NONNUMERIC_VANILLA  4.3.2.1:url
  Do Not Expect Symbol With Option  URIBL_NONNUMERIC_VANILLA  1.2.3.4:url

NONNUMERIC URLS WITH EVERYTHING
  Scan File  ${RSPAMD_TESTDIR}/messages/numeric_urls.eml
  ...  Settings={symbols_enabled = [URIBL_NONNUMERIC_EVERYTHING]}
  # Content
  Expect Symbol With Option  URIBL_NONNUMERIC_EVERYTHING  example.com:url
  # Image
  Expect Symbol With Option  URIBL_NONNUMERIC_EVERYTHING  judo.za.org:url
  # URL
  Expect Symbol With Option  URIBL_NONNUMERIC_EVERYTHING  example.org:url
  # Numeric
  Do Not Expect Symbol With Option  URIBL_NONNUMERIC_EVERYTHING  4.3.2.1:url
  Do Not Expect Symbol With Option  URIBL_NONNUMERIC_EVERYTHING  1.2.3.4:url
