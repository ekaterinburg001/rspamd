/*-
 * Copyright 2016 Vsevolod Stakhov
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "lua_common.h"
#include "libmime/message.h"
#include "libmime/lang_detection.h"
#include "libstat/stat_api.h"

/* Textpart methods */
/***
 * @module rspamd_textpart
 * This module provides different methods to manipulate text parts data. Text parts
 * could be obtained from the `rspamd_task` by using of method `task:get_text_parts()`
@example
rspamd_config.R_EMPTY_IMAGE = function (task)
	parts = task:get_text_parts()
	if parts then
		for _,part in ipairs(parts) do
			if part:is_empty() then
				texts = task:get_texts()
				if texts then
					return true
				end
				return false
			end
		end
	end
	return false
end
 */

/***
 * @method text_part:is_utf()
 * Return TRUE if part is a valid utf text
 * @return {boolean} true if part is valid `UTF8` part
 */
LUA_FUNCTION_DEF (textpart, is_utf);

/***
 * @method text_part:has_8bit_raw()
 * Return TRUE if a part has raw 8bit characters
 * @return {boolean} true if a part has raw 8bit characters
 */
LUA_FUNCTION_DEF (textpart, has_8bit_raw);

/***
 * @method text_part:has_8bit()
 * Return TRUE if a part has raw 8bit characters
 * @return {boolean} true if a part has encoded 8bit characters
 */
LUA_FUNCTION_DEF (textpart, has_8bit);

/***
 * @method text_part:get_content([type])
 * Get the text of the part (html tags stripped). Optional `type` defines type of content to get:
 * - `content` (default): utf8 content with HTML tags stripped and newlines preserved
 * - `content_oneline`: utf8 content with HTML tags and newlines stripped
 * - `raw`: raw content, not mime decoded nor utf8 converted
 * - `raw_parsed`: raw content, mime decoded, not utf8 converted
 * - `raw_utf`: raw content, mime decoded, utf8 converted (but with HTML tags and newlines)
 * @return {text} `UTF8` encoded content of the part (zero-copy if not converted to a lua string)
 */
LUA_FUNCTION_DEF (textpart, get_content);
/***
 * @method text_part:get_raw_content()
 * Get the original text of the part
 * @return {text} `UTF8` encoded content of the part (zero-copy if not converted to a lua string)
 */
LUA_FUNCTION_DEF (textpart, get_raw_content);
/***
 * @method text_part:get_content_oneline()
 *Get the text of the part (html tags and newlines stripped)
 * @return {text} `UTF8` encoded content of the part (zero-copy if not converted to a lua string)
 */
LUA_FUNCTION_DEF (textpart, get_content_oneline);
/***
 * @method text_part:get_length()
 * Get length of the text of the part
 * @return {integer} length of part in **bytes**
 */
LUA_FUNCTION_DEF (textpart, get_length);
/***
 * @method mime_part:get_raw_length()
 * Get length of the **raw** content of the part (e.g. HTML with tags unstripped)
 * @return {integer} length of part in **bytes**
 */
LUA_FUNCTION_DEF (textpart, get_raw_length);
/***
 * @method mime_part:get_urls_length()
 * Get length of the urls within the part
 * @return {integer} length of urls in **bytes**
 */
LUA_FUNCTION_DEF (textpart, get_urls_length);
/***
 * @method mime_part:get_lines_count()
 * Get lines number in the part
 * @return {integer} number of lines in the part
 */
LUA_FUNCTION_DEF (textpart, get_lines_count);
/***
 * @method mime_part:get_stats()
 * Returns a table with the following data:
 * - `lines`: number of lines
 * - `spaces`: number of spaces
 * - `double_spaces`: double spaces
 * - `empty_lines`: number of empty lines
 * - `non_ascii_characters`: number of non ascii characters
 * - `ascii_characters`: number of ascii characters
 * @return {table} table of stats
 */
LUA_FUNCTION_DEF (textpart, get_stats);
/***
 * @method mime_part:get_words_count()
 * Get words number in the part
 * @return {integer} number of words in the part
 */
LUA_FUNCTION_DEF (textpart, get_words_count);

/***
 * @method mime_part:get_words()
 * Get words in the part
 * @return {table/strings} words in the part
 */
LUA_FUNCTION_DEF (textpart, get_words);

/***
 * @method text_part:is_empty()
 * Returns `true` if the specified part is empty
 * @return {bool} whether a part is empty
 */
LUA_FUNCTION_DEF (textpart, is_empty);
/***
 * @method text_part:is_html()
 * Returns `true` if the specified part has HTML content
 * @return {bool} whether a part is HTML part
 */
LUA_FUNCTION_DEF (textpart, is_html);
/***
 * @method text_part:get_html()
 * Returns html content of the specified part
 * @return {html} html content
 */
LUA_FUNCTION_DEF (textpart, get_html);
/***
 * @method text_part:get_language()
 * Returns the code of the most used unicode script in the text part. Does not work with raw parts
 * @return {string} short abbreviation (such as `ru`) for the script's language
 */
LUA_FUNCTION_DEF (textpart, get_language);
/***
 * @method text_part:get_languages()
 * Returns array of tables of all languages detected for a part:
 * - 'code': language code (short string)
 * - 'prob': logarithm of probability
 * @return {array|tables} all languages detected for the part
 */
LUA_FUNCTION_DEF (textpart, get_languages);
/***
 * @method text_part:get_mimepart()
 * Returns the mime part object corresponding to this text part
 * @return {mimepart} mimepart object
 */
LUA_FUNCTION_DEF (textpart, get_mimepart);

static const struct luaL_reg textpartlib_m[] = {
	LUA_INTERFACE_DEF (textpart, is_utf),
	LUA_INTERFACE_DEF (textpart, has_8bit_raw),
	LUA_INTERFACE_DEF (textpart, has_8bit),
	LUA_INTERFACE_DEF (textpart, get_content),
	LUA_INTERFACE_DEF (textpart, get_raw_content),
	LUA_INTERFACE_DEF (textpart, get_content_oneline),
	LUA_INTERFACE_DEF (textpart, get_length),
	LUA_INTERFACE_DEF (textpart, get_raw_length),
	LUA_INTERFACE_DEF (textpart, get_urls_length),
	LUA_INTERFACE_DEF (textpart, get_lines_count),
	LUA_INTERFACE_DEF (textpart, get_words_count),
	LUA_INTERFACE_DEF (textpart, get_words),
	LUA_INTERFACE_DEF (textpart, is_empty),
	LUA_INTERFACE_DEF (textpart, is_html),
	LUA_INTERFACE_DEF (textpart, get_html),
	LUA_INTERFACE_DEF (textpart, get_language),
	LUA_INTERFACE_DEF (textpart, get_languages),
	LUA_INTERFACE_DEF (textpart, get_mimepart),
	LUA_INTERFACE_DEF (textpart, get_stats),
	{"__tostring", rspamd_lua_class_tostring},
	{NULL, NULL}
};

/* Mimepart methods */

/***
 * @module rspamd_mimepart
 * This module provides access to mime parts found in a message
@example
rspamd_config.MISSING_CONTENT_TYPE = function(task)
	local parts = task:get_parts()
	if parts and #parts > 1 then
		-- We have more than one part
		for _,p in ipairs(parts) do
			local ct = p:get_header('Content-Type')
			-- And some parts have no Content-Type header
			if not ct then
				return true
			end
		end
	end
	return false
end
 */

/***
 * @method mime_part:get_header(name[, case_sensitive])
 * Get decoded value of a header specified with optional case_sensitive flag.
 * By default headers are searched in caseless matter.
 * @param {string} name name of header to get
 * @param {boolean} case_sensitive case sensitiveness flag to search for a header
 * @return {string} decoded value of a header
 */
LUA_FUNCTION_DEF (mimepart, get_header);
/***
 * @method mime_part:get_header_raw(name[, case_sensitive])
 * Get raw value of a header specified with optional case_sensitive flag.
 * By default headers are searched in caseless matter.
 * @param {string} name name of header to get
 * @param {boolean} case_sensitive case sensitiveness flag to search for a header
 * @return {string} raw value of a header
 */
LUA_FUNCTION_DEF (mimepart, get_header_raw);
/***
 * @method mime_part:get_header_full(name[, case_sensitive])
 * Get raw value of a header specified with optional case_sensitive flag.
 * By default headers are searched in caseless matter. This method returns more
 * information about the header as a list of tables with the following structure:
 *
 * - `name` - name of a header
 * - `value` - raw value of a header
 * - `decoded` - decoded value of a header
 * - `tab_separated` - `true` if a header and a value are separated by `tab` character
 * - `empty_separator` - `true` if there are no separator between a header and a value
 * @param {string} name name of header to get
 * @param {boolean} case_sensitive case sensitiveness flag to search for a header
 * @return {list of tables} all values of a header as specified above
@example
function check_header_delimiter_tab(task, header_name)
	for _,rh in ipairs(task:get_header_full(header_name)) do
		if rh['tab_separated'] then return true end
	end
	return false
end
 */
LUA_FUNCTION_DEF (mimepart, get_header_full);
/***
 * @method mime_part:get_content()
 * Get the parsed content of part
 * @return {text} opaque text object (zero-copy if not casted to lua string)
 */
LUA_FUNCTION_DEF (mimepart, get_content);
/***
 * @method mime_part:get_raw_content()
 * Get the raw content of part
 * @return {text} opaque text object (zero-copy if not casted to lua string)
 */
LUA_FUNCTION_DEF (mimepart, get_raw_content);
/***
 * @method mime_part:get_length()
 * Get length of the content of the part
 * @return {integer} length of part in **bytes**
 */
LUA_FUNCTION_DEF (mimepart, get_length);
/***
 * @method mime_part:get_type()
 * Extract content-type string of the mime part
 * @return {string,string} content type in form 'type','subtype'
 */
LUA_FUNCTION_DEF (mimepart, get_type);

/***
 * @method mime_part:get_type_full()
 * Extract content-type string of the mime part with all attributes
 * @return {string,string,table} content type in form 'type','subtype', {attrs}
 */
LUA_FUNCTION_DEF (mimepart, get_type_full);

/***
 * @method mime_part:get_cte()
 * Extract content-transfer-encoding for a part
 * @return {string} content transfer encoding (e.g. `base64` or `7bit`)
 */
LUA_FUNCTION_DEF (mimepart, get_cte);

/***
 * @method mime_part:get_filename()
 * Extract filename associated with mime part if it is an attachment
 * @return {string} filename or `nil` if no file is associated with this part
 */
LUA_FUNCTION_DEF (mimepart, get_filename);
/***
 * @method mime_part:is_image()
 * Returns true if mime part is an image
 * @return {bool} true if a part is an image
 */
LUA_FUNCTION_DEF (mimepart, is_image);
/***
 * @method mime_part:get_image()
 * Returns rspamd_image structure associated with this part. This structure has
 * the following methods:
 *
 * * `get_width` - return width of an image in pixels
 * * `get_height` - return height of an image in pixels
 * * `get_type` - return string representation of image's type (e.g. 'jpeg')
 * * `get_filename` - return string with image's file name
 * * `get_size` - return size in bytes
 * @return {rspamd_image} image structure or nil if a part is not an image
 */
LUA_FUNCTION_DEF (mimepart, get_image);
/***
 * @method mime_part:is_archive()
 * Returns true if mime part is an archive
 * @return {bool} true if a part is an archive
 */
LUA_FUNCTION_DEF (mimepart, is_archive);

/***
 * @method mime_part:get_archive()
 * Returns rspamd_archive structure associated with this part. This structure has
 * the following methods:
 *
 * * `get_files` - return list of strings with filenames inside archive
 * * `get_files_full` - return list of tables with all information about files
 * * `is_encrypted` - return true if an archive is encrypted
 * * `get_type` - return string representation of image's type (e.g. 'zip')
 * * `get_filename` - return string with archive's file name
 * * `get_size` - return size in bytes
 * @return {rspamd_archive} archive structure or nil if a part is not an archive
 */
LUA_FUNCTION_DEF (mimepart, get_archive);
/***
 * @method mime_part:is_multipart()
 * Returns true if mime part is a multipart part
 * @return {bool} true if a part is is a multipart part
 */
LUA_FUNCTION_DEF (mimepart, is_multipart);
/***
 * @method mime_part:get_children()
 * Returns rspamd_mimepart table of part's childer. Returns nil if mime part is not multipart
 * or a message part.
 * @return {rspamd_mimepart} table of children
 */
LUA_FUNCTION_DEF (mimepart, get_children);
/***
 * @method mime_part:is_text()
 * Returns true if mime part is a text part
 * @return {bool} true if a part is a text part
 */
LUA_FUNCTION_DEF (mimepart, is_text);
/***
 * @method mime_part:get_text()
 * Returns rspamd_textpart structure associated with this part.
 * @return {rspamd_textpart} textpart structure or nil if a part is not an text
 */
LUA_FUNCTION_DEF (mimepart, get_text);

/***
 * @method mime_part:get_digest()
 * Returns the unique digest for this mime part
 * @return {string} 128 characters hex string with digest of the part
 */
LUA_FUNCTION_DEF (mimepart, get_digest);
/***
 * @method mime_part:is_broken()
 * Returns true if mime part has incorrectly specified content type
 * @return {bool} true if a part has bad content type
 */
LUA_FUNCTION_DEF (mimepart, is_broken);
/***
 * @method mime_part:headers_foreach(callback, [params])
 * This method calls `callback` for each header that satisfies some condition.
 * By default, all headers are iterated unless `callback` returns `true`. Nil or
 * false means continue of iterations.
 * Params could be as following:
 *
 * - `full`: header value is full table of all attributes @see task:get_header_full for details
 * - `regexp`: return headers that satisfies the specified regexp
 * @param {function} callback function from header name and header value
 * @param {table} params optional parameters
 */
LUA_FUNCTION_DEF (mimepart, headers_foreach);

static const struct luaL_reg mimepartlib_m[] = {
	LUA_INTERFACE_DEF (mimepart, get_content),
	LUA_INTERFACE_DEF (mimepart, get_raw_content),
	LUA_INTERFACE_DEF (mimepart, get_length),
	LUA_INTERFACE_DEF (mimepart, get_type),
	LUA_INTERFACE_DEF (mimepart, get_type_full),
	LUA_INTERFACE_DEF (mimepart, get_cte),
	LUA_INTERFACE_DEF (mimepart, get_filename),
	LUA_INTERFACE_DEF (mimepart, get_header),
	LUA_INTERFACE_DEF (mimepart, get_header_raw),
	LUA_INTERFACE_DEF (mimepart, get_header_full),
	LUA_INTERFACE_DEF (mimepart, is_image),
	LUA_INTERFACE_DEF (mimepart, get_image),
	LUA_INTERFACE_DEF (mimepart, is_archive),
	LUA_INTERFACE_DEF (mimepart, get_archive),
	LUA_INTERFACE_DEF (mimepart, is_multipart),
	LUA_INTERFACE_DEF (mimepart, get_children),
	LUA_INTERFACE_DEF (mimepart, is_text),
	LUA_INTERFACE_DEF (mimepart, is_broken),
	LUA_INTERFACE_DEF (mimepart, get_text),
	LUA_INTERFACE_DEF (mimepart, get_digest),
	LUA_INTERFACE_DEF (mimepart, headers_foreach),
	{"__tostring", rspamd_lua_class_tostring},
	{NULL, NULL}
};


static struct rspamd_mime_text_part *
lua_check_textpart (lua_State * L)
{
	void *ud = rspamd_lua_check_udata (L, 1, "rspamd{textpart}");
	luaL_argcheck (L, ud != NULL, 1, "'textpart' expected");
	return ud ? *((struct rspamd_mime_text_part **)ud) : NULL;
}

static struct rspamd_mime_part *
lua_check_mimepart (lua_State * L)
{
	void *ud = rspamd_lua_check_udata (L, 1, "rspamd{mimepart}");
	luaL_argcheck (L, ud != NULL, 1, "'mimepart' expected");
	return ud ? *((struct rspamd_mime_part **)ud) : NULL;
}


static gint
lua_textpart_is_utf (lua_State * L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);

	if (part == NULL || IS_PART_EMPTY (part)) {
		lua_pushboolean (L, FALSE);
		return 1;
	}

	lua_pushboolean (L, IS_PART_UTF (part));

	return 1;
}


static gint
lua_textpart_has_8bit_raw (lua_State * L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);

	if (part) {
		if (part->flags & RSPAMD_MIME_TEXT_PART_FLAG_8BIT) {
			lua_pushboolean (L, TRUE);
		}
		else {
			lua_pushboolean (L, FALSE);
		}
	}
	else {
		return luaL_error (L, "invalid arguments");
	}

	return 1;
}

static gint
lua_textpart_has_8bit (lua_State * L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);

	if (part) {
		if (part->flags & RSPAMD_MIME_TEXT_PART_FLAG_8BIT_ENCODED) {
			lua_pushboolean (L, TRUE);
		}
		else {
			lua_pushboolean (L, FALSE);
		}
	}
	else {
		return luaL_error (L, "invalid arguments");
	}

	return 1;
}


static gint
lua_textpart_get_content (lua_State * L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);
	struct rspamd_lua_text *t;
	gsize len;
	const gchar *start, *type = NULL;

	if (part == NULL || IS_PART_EMPTY (part)) {
		lua_pushnil (L);
		return 1;
	}

	if (lua_type (L, 2) == LUA_TSTRING) {
		type = lua_tostring (L, 2);
	}

	t = lua_newuserdata (L, sizeof (*t));
	rspamd_lua_setclass (L, "rspamd{text}", -1);

	if (!type) {
		start = part->content->data;
		len = part->content->len;
	}
	else if (strcmp (type, "content") == 0) {
		start = part->content->data;
		len = part->content->len;
	}
	else if (strcmp (type, "content_oneline") == 0) {
		start = part->stripped_content->data;
		len = part->stripped_content->len;
	}
	else if (strcmp (type, "raw_parsed") == 0) {
		start = part->parsed.begin;
		len = part->parsed.len;
	}
	else if (strcmp (type, "raw_utf") == 0) {
		start = part->utf_raw_content->data;
		len = part->utf_raw_content->len;
	}
	else if (strcmp (type, "raw") == 0) {
		start = part->raw.begin;
		len = part->raw.len;
	}
	else {
		return luaL_error (L, "invalid content type: %s", type);
	}

	t->start = start;
	t->len = len;
	t->flags = 0;

	return 1;
}

static gint
lua_textpart_get_raw_content (lua_State * L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);
	struct rspamd_lua_text *t;

	if (part == NULL || IS_PART_EMPTY (part)) {
		lua_pushnil (L);
		return 1;
	}

	t = lua_newuserdata (L, sizeof (*t));
	rspamd_lua_setclass (L, "rspamd{text}", -1);
	t->start = part->raw.begin;
	t->len = part->raw.len;
	t->flags = 0;

	return 1;
}

static gint
lua_textpart_get_content_oneline (lua_State * L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);
	struct rspamd_lua_text *t;

	if (part == NULL || IS_PART_EMPTY (part)) {
		lua_pushnil (L);
		return 1;
	}

	t = lua_newuserdata (L, sizeof (*t));
	rspamd_lua_setclass (L, "rspamd{text}", -1);
	t->start = part->stripped_content->data;
	t->len = part->stripped_content->len;
	t->flags = 0;

	return 1;
}

static gint
lua_textpart_get_length (lua_State * L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);

	if (part == NULL) {
		lua_pushnil (L);
		return 1;
	}

	if (IS_PART_EMPTY (part) || part->content == NULL) {
		lua_pushnumber (L, 0);
	}
	else {
		lua_pushnumber (L, part->content->len);
	}

	return 1;
}

static gint
lua_textpart_get_raw_length (lua_State * L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);

	if (part == NULL) {
		lua_pushnil (L);
		return 1;
	}

	lua_pushnumber (L, part->raw.len);

	return 1;
}

static gint
lua_textpart_get_urls_length (lua_State * L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);
	GList *cur;
	guint total = 0;
	struct rspamd_process_exception *ex;

	if (part == NULL) {
		lua_pushnil (L);
		return 1;
	}

	for (cur = part->exceptions; cur != NULL; cur = g_list_next (cur)) {
		ex = cur->data;

		if (ex->type == RSPAMD_EXCEPTION_URL) {
			total += ex->len;
		}
	}

	lua_pushnumber (L, total);

	return 1;
}

static gint
lua_textpart_get_lines_count (lua_State * L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);

	if (part == NULL) {
		lua_pushnil (L);
		return 1;
	}

	if (IS_PART_EMPTY (part)) {
		lua_pushnumber (L, 0);
	}
	else {
		lua_pushnumber (L, part->nlines);
	}

	return 1;
}

static gint
lua_textpart_get_words_count (lua_State *L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);

	if (part == NULL) {
		lua_pushnil (L);
		return 1;
	}

	if (IS_PART_EMPTY (part) || part->normalized_words == NULL) {
		lua_pushnumber (L, 0);
	}
	else {
		lua_pushnumber (L, part->normalized_words->len);
	}

	return 1;
}

static gint
lua_textpart_get_words (lua_State *L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);
	rspamd_stat_token_t *w;
	guint i;

	if (part == NULL) {
		return luaL_error (L, "invalid arguments");
	}

	if (IS_PART_EMPTY (part) || part->normalized_words == NULL) {
		lua_createtable (L, 0, 0);
	}
	else {
		lua_createtable (L, part->normalized_words->len, 0);

		for (i = 0; i < part->normalized_words->len; i ++) {
			w = &g_array_index (part->normalized_words, rspamd_stat_token_t, i);

			lua_pushlstring (L, w->begin, w->len);
			lua_rawseti (L, -2, i + 1);
		}
	}

	return 1;
}

static gint
lua_textpart_is_empty (lua_State * L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);

	if (part == NULL) {
		lua_pushnil (L);
		return 1;
	}

	lua_pushboolean (L, IS_PART_EMPTY (part));

	return 1;
}

static gint
lua_textpart_is_html (lua_State * L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);

	if (part == NULL) {
		lua_pushnil (L);
		return 1;
	}

	lua_pushboolean (L, IS_PART_HTML (part));

	return 1;
}

static gint
lua_textpart_get_html (lua_State * L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);
	struct html_content **phc;

	if (part == NULL || part->html == NULL) {
		lua_pushnil (L);
	}
	else {
		phc = lua_newuserdata (L, sizeof (*phc));
		rspamd_lua_setclass (L, "rspamd{html}", -1);
		*phc = part->html;
	}

	return 1;
}

static gint
lua_textpart_get_language (lua_State * L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);

	if (part != NULL) {
		if (part->language != NULL && part->language[0] != '\0') {
			lua_pushstring (L, part->language);
			return 1;
		}
		else {
			lua_pushnil (L);
		}
	}
	else {
		return luaL_error (L, "invalid arguments");
	}

	return 1;
}

static gint
lua_textpart_get_languages (lua_State * L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);
	guint i;
	struct rspamd_lang_detector_res *cur;

	if (part != NULL) {
		if (part->languages != NULL) {
			lua_createtable (L, part->languages->len, 0);

			PTR_ARRAY_FOREACH (part->languages, i, cur) {
				lua_createtable (L, 0, 2);
				lua_pushstring (L, "code");
				lua_pushstring (L, cur->lang);
				lua_settable (L, -3);
				lua_pushstring (L, "prob");
				lua_pushnumber (L, cur->prob);
				lua_settable (L, -3);

				lua_rawseti (L, -2, i + 1);
			}
		}
		else {
			lua_newtable (L);
		}
	}
	else {
		luaL_error (L, "invalid arguments");
	}

	return 1;
}

static gint
lua_textpart_get_mimepart (lua_State * L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);
	struct rspamd_mime_part **pmime;

	if (part != NULL) {
		if (part->mime_part != NULL) {
			pmime = lua_newuserdata (L, sizeof (struct rspamd_mime_part *));
			rspamd_lua_setclass (L, "rspamd{mimepart}", -1);
			*pmime = part->mime_part;

			return 1;
		}
	}

	lua_pushnil (L);
	return 1;
}

/***
 * @method mime_part:get_stats()
 * Returns a table with the following data:
 * -
 * - `lines`: number of lines
 * - `spaces`: number of spaces
 * - `double_spaces`: double spaces
 * - `empty_lines`: number of empty lines
 * - `non_ascii_characters`: number of non ascii characters
 * - `ascii_characters`: number of ascii characters
 * @return {table} table of stats
 */
static gint
lua_textpart_get_stats (lua_State * L)
{
	struct rspamd_mime_text_part *part = lua_check_textpart (L);

	if (part != NULL) {
		lua_createtable (L, 0, 9);

		lua_pushstring (L, "lines");
		lua_pushnumber (L, part->nlines);
		lua_settable (L, -3);
		lua_pushstring (L, "empty_lines");
		lua_pushnumber (L, part->empty_lines);
		lua_settable (L, -3);
		lua_pushstring (L, "spaces");
		lua_pushnumber (L, part->spaces);
		lua_settable (L, -3);
		lua_pushstring (L, "non_spaces");
		lua_pushnumber (L, part->non_spaces);
		lua_settable (L, -3);
		lua_pushstring (L, "double_spaces");
		lua_pushnumber (L, part->double_spaces);
		lua_settable (L, -3);
		lua_pushstring (L, "ascii_characters");
		lua_pushnumber (L, part->ascii_chars);
		lua_settable (L, -3);
		lua_pushstring (L, "non_ascii_characters");
		lua_pushnumber (L, part->non_ascii_chars);
		lua_settable (L, -3);
		lua_pushstring (L, "capital_letters");
		lua_pushnumber (L, part->capital_letters);
		lua_settable (L, -3);
		lua_pushstring (L, "numeric_characters");
		lua_pushnumber (L, part->numeric_characters);
		lua_settable (L, -3);
	}
	else {
		return luaL_error (L, "invalid arguments");
	}

	return 1;
}

/* Mimepart implementation */

static gint
lua_mimepart_get_content (lua_State * L)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);
	struct rspamd_lua_text *t;

	if (part == NULL) {
		lua_pushnil (L);
		return 1;
	}

	t = lua_newuserdata (L, sizeof (*t));
	rspamd_lua_setclass (L, "rspamd{text}", -1);
	t->start = part->parsed_data.begin;
	t->len = part->parsed_data.len;
	t->flags = 0;

	return 1;
}

static gint
lua_mimepart_get_raw_content (lua_State * L)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);
	struct rspamd_lua_text *t;

	if (part == NULL) {
		lua_pushnil (L);
		return 1;
	}

	t = lua_newuserdata (L, sizeof (*t));
	rspamd_lua_setclass (L, "rspamd{text}", -1);
	t->start = part->raw_data.begin;
	t->len = part->raw_data.len;
	t->flags = 0;

	return 1;
}

static gint
lua_mimepart_get_length (lua_State * L)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);

	if (part == NULL) {
		lua_pushnil (L);
		return 1;
	}

	lua_pushnumber (L, part->parsed_data.len);

	return 1;
}

static gint
lua_mimepart_get_type_common (lua_State * L, gboolean full)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);
	GHashTableIter it;
	gpointer k, v;
	struct rspamd_content_type_param *param;

	if (part == NULL) {
		lua_pushnil (L);
		lua_pushnil (L);
		return 2;
	}

	lua_pushlstring (L, part->ct->type.begin, part->ct->type.len);
	lua_pushlstring (L, part->ct->subtype.begin, part->ct->subtype.len);

	if (!full) {
		return 2;
	}

	lua_createtable (L, 0, 2 + (part->ct->attrs ?
			g_hash_table_size (part->ct->attrs) : 0));

	if (part->ct->charset.len > 0) {
		lua_pushstring (L, "charset");
		lua_pushlstring (L, part->ct->charset.begin, part->ct->charset.len);
		lua_settable (L, -3);
	}

	if (part->ct->boundary.len > 0) {
		lua_pushstring (L, "charset");
		lua_pushlstring (L, part->ct->boundary.begin, part->ct->boundary.len);
		lua_settable (L, -3);
	}

	if (part->ct->attrs) {
		g_hash_table_iter_init (&it, part->ct->attrs);

		while (g_hash_table_iter_next (&it, &k, &v)) {
			param = v;

			if (param->name.len > 0 && param->name.len > 0) {
				/* TODO: think about multiple values here */
				lua_pushlstring (L, param->name.begin, param->name.len);
				lua_pushlstring (L, param->value.begin, param->value.len);
				lua_settable (L, -3);
			}
		}
	}

	return 3;
}

static gint
lua_mimepart_get_type (lua_State * L)
{
	return lua_mimepart_get_type_common (L, FALSE);
}

static gint
lua_mimepart_get_type_full (lua_State * L)
{
	return lua_mimepart_get_type_common (L, TRUE);
}

static gint
lua_mimepart_get_cte (lua_State * L)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);

	if (part == NULL) {
		lua_pushnil (L);
		return 1;
	}

	lua_pushstring (L, rspamd_cte_to_string (part->cte));

	return 1;
}

static gint
lua_mimepart_get_filename (lua_State * L)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);

	if (part == NULL || part->cd == NULL || part->cd->filename.len == 0) {
		lua_pushnil (L);
		return 1;
	}

	lua_pushlstring (L, part->cd->filename.begin, part->cd->filename.len);

	return 1;
}

static gint
lua_mimepart_get_header_common (lua_State *L, gboolean full, gboolean raw)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);
	const gchar *name;
	GPtrArray *ar;

	name = luaL_checkstring (L, 2);

	if (name && part) {

		ar = rspamd_message_get_header_from_hash (part->raw_headers, NULL,
				name, FALSE);

		return rspamd_lua_push_header_array (L, ar, full, raw);
	}

	lua_pushnil (L);

	return 1;
}

static gint
lua_mimepart_get_header_full (lua_State * L)
{
	return lua_mimepart_get_header_common (L, TRUE, TRUE);
}

static gint
lua_mimepart_get_header (lua_State * L)
{
	return lua_mimepart_get_header_common (L, FALSE, FALSE);
}

static gint
lua_mimepart_get_header_raw (lua_State * L)
{
	return lua_mimepart_get_header_common (L, FALSE, TRUE);
}

static gint
lua_mimepart_is_image (lua_State * L)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);

	if (part == NULL) {
		return luaL_error (L, "invalid arguments");
	}

	lua_pushboolean (L, (part->flags & RSPAMD_MIME_PART_IMAGE) ? true : false);

	return 1;
}

static gint
lua_mimepart_is_archive (lua_State * L)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);

	if (part == NULL) {
		return luaL_error (L, "invalid arguments");
	}

	lua_pushboolean (L, (part->flags & RSPAMD_MIME_PART_ARCHIVE) ? true : false);

	return 1;
}

static gint
lua_mimepart_is_multipart (lua_State * L)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);

	if (part == NULL) {
		return luaL_error (L, "invalid arguments");
	}

	lua_pushboolean (L, IS_CT_MULTIPART (part->ct) ? true : false);

	return 1;
}

static gint
lua_mimepart_is_text (lua_State * L)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);

	if (part == NULL) {
		return luaL_error (L, "invalid arguments");
	}

	lua_pushboolean (L, (part->flags & RSPAMD_MIME_PART_TEXT) ? true : false);

	return 1;
}

static gint
lua_mimepart_is_broken (lua_State * L)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);

	if (part == NULL) {
		return luaL_error (L, "invalid arguments");
	}

	if (part->ct) {
		lua_pushboolean (L, (part->ct->flags & RSPAMD_CONTENT_TYPE_BROKEN) ?
				true : false);
	}
	else {
		lua_pushboolean (L, true);
	}

	return 1;
}

static gint
lua_mimepart_get_image (lua_State * L)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);
	struct rspamd_image **pimg;

	if (part == NULL) {
		return luaL_error (L, "invalid arguments");
	}

	if (!(part->flags & RSPAMD_MIME_PART_IMAGE) || part->specific.img == NULL) {
		lua_pushnil (L);
	}
	else {
		pimg = lua_newuserdata (L, sizeof (*pimg));
		*pimg = part->specific.img;
		rspamd_lua_setclass (L, "rspamd{image}", -1);
	}

	return 1;
}

static gint
lua_mimepart_get_archive (lua_State * L)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);
	struct rspamd_archive **parch;

	if (part == NULL) {
		return luaL_error (L, "invalid arguments");
	}

	if (!(part->flags & RSPAMD_MIME_PART_ARCHIVE) || part->specific.arch == NULL) {
		lua_pushnil (L);
	}
	else {
		parch = lua_newuserdata (L, sizeof (*parch));
		*parch = part->specific.arch;
		rspamd_lua_setclass (L, "rspamd{archive}", -1);
	}

	return 1;
}

static gint
lua_mimepart_get_children (lua_State * L)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);
	struct rspamd_mime_part **pcur, *cur;
	guint i;

	if (part == NULL) {
		return luaL_error (L, "invalid arguments");
	}

	if (!IS_CT_MULTIPART (part->ct) || part->specific.mp.children == NULL) {
		lua_pushnil (L);
	}
	else {
		lua_createtable (L, part->specific.mp.children->len, 0);

		PTR_ARRAY_FOREACH (part->specific.mp.children, i, cur) {
			pcur = lua_newuserdata (L, sizeof (*pcur));
			*pcur = cur;
			rspamd_lua_setclass (L, "rspamd{mimepart}", -1);
			lua_rawseti (L, -2, i + 1);
		}
	}

	return 1;
}


static gint
lua_mimepart_get_text (lua_State * L)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);
	struct rspamd_mime_text_part **ppart;

	if (part == NULL) {
		return luaL_error (L, "invalid arguments");
	}

	if (!(part->flags & RSPAMD_MIME_PART_TEXT) || part->specific.txt == NULL) {
		lua_pushnil (L);
	}
	else {
		ppart = lua_newuserdata (L, sizeof (*ppart));
		*ppart = part->specific.txt;
		rspamd_lua_setclass (L, "rspamd{textpart}", -1);
	}

	return 1;
}

static gint
lua_mimepart_get_digest (lua_State * L)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);
	gchar digestbuf[rspamd_cryptobox_HASHBYTES * 2 + 1];

	if (part == NULL) {
		return luaL_error (L, "invalid arguments");
	}

	memset (digestbuf, 0, sizeof (digestbuf));
	rspamd_encode_hex_buf (part->digest, sizeof (part->digest),
			digestbuf, sizeof (digestbuf));
	lua_pushstring (L, digestbuf);

	return 1;
}

static gint
lua_mimepart_headers_foreach (lua_State *L)
{
	struct rspamd_mime_part *part = lua_check_mimepart (L);
	gboolean full = FALSE, raw = FALSE;
	struct rspamd_lua_regexp *re = NULL;
	GList *cur;
	struct rspamd_mime_header *hdr;
	gint old_top;

	if (part && lua_isfunction (L, 2)) {
		if (lua_istable (L, 3)) {
			lua_pushstring (L, "full");
			lua_gettable (L, 3);

			if (lua_isboolean (L, -1)) {
				full = lua_toboolean (L, -1);
			}

			lua_pop (L, 1);

			lua_pushstring (L, "raw");
			lua_gettable (L, 3);

			if (lua_isboolean (L, -1)) {
				raw = lua_toboolean (L, -1);
			}

			lua_pop (L, 1);

			lua_pushstring (L, "regexp");
			lua_gettable (L, 3);

			if (lua_isuserdata (L, -1)) {
				re = *(struct rspamd_lua_regexp **)
						rspamd_lua_check_udata (L, -1, "rspamd{regexp}");
			}

			lua_pop (L, 1);
		}

		if (part->headers_order) {
			cur = part->headers_order->head;

			while (cur) {
				hdr = cur->data;

				if (re && re->re) {
					if (!rspamd_regexp_match (re->re, hdr->name,
							strlen (hdr->name),FALSE)) {
						cur = g_list_next (cur);
						continue;
					}
				}

				old_top = lua_gettop (L);
				lua_pushvalue (L, 2);
				lua_pushstring (L, hdr->name);
				rspamd_lua_push_header (L, hdr, full, raw);

				if (lua_pcall (L, 2, LUA_MULTRET, 0) != 0) {
					msg_err ("call to header_foreach failed: %s",
							lua_tostring (L, -1));
					lua_settop (L, old_top);
					break;
				}
				else {
					if (lua_gettop (L) > old_top) {
						if (lua_isboolean (L, old_top + 1)) {
							if (lua_toboolean (L, old_top + 1)) {
								lua_settop (L, old_top);
								break;
							}
						}
					}
				}

				lua_settop (L, old_top);
				cur = g_list_next (cur);
			}
		}
	}

	return 0;
}

void
luaopen_textpart (lua_State * L)
{
	rspamd_lua_new_class (L, "rspamd{textpart}", textpartlib_m);
	lua_pop (L, 1);
}

void
luaopen_mimepart (lua_State * L)
{
	rspamd_lua_new_class (L, "rspamd{mimepart}", mimepartlib_m);
	lua_pop (L, 1);
}

