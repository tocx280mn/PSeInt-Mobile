#include <string>

extern "C" {
#include "hoedown/document.h"
#include "hoedown/buffer.h"
#include "hoedown/html.h"
}

#include "hoewrap.h"

static const hoedown_extensions extensions = static_cast<hoedown_extensions>(
		(HOEDOWN_EXT_TABLES)| // Parse PHP-Markdown style tables
		(HOEDOWN_EXT_FENCED_CODE)| // Parse fenced code blocks
//		(HOEDOWN_EXT_FOOTNOTES)| // Parse footnotes
		(HOEDOWN_EXT_AUTOLINK)| // Automatically turn safe URLs into links
		(HOEDOWN_EXT_STRIKETHROUGH)| // Parse ~~stikethrough~~ spans
		(HOEDOWN_EXT_UNDERLINE)| // Parse _underline_ instead of emphasis
		(HOEDOWN_EXT_HIGHLIGHT)| // Parse ==highlight== spans
		(HOEDOWN_EXT_QUOTE)| // Render \"quotes\" as <q>quotes</q>
		(HOEDOWN_EXT_SUPERSCRIPT)| // Parse super^script
//		(HOEDOWN_EXT_MATH)| // Parse TeX $$math$$ syntax, Kramdown style
		(HOEDOWN_EXT_NO_INTRA_EMPHASIS)| // Disable emphasis_between_words
		(HOEDOWN_EXT_SPACE_HEADERS)| // Require a space after '#' in headers
//		(HOEDOWN_EXT_MATH_EXPLICIT)| // Instead of guessing by context, parse $inline math$ and $$always block math$$ (requires --math)
//		(HOEDOWN_EXT_DISABLE_INDENTED_CODE)| // Don't parse indented code blocks
		0);

static const hoedown_html_flags flags = static_cast<hoedown_html_flags>(
//		(HOEDOWN_HTML_SKIP_HTML)| // Strip all HTML tags
//		(HOEDOWN_HTML_ESCAPE)| // Escape all HTML
//		(HOEDOWN_HTML_HARD_WRAP)| // Render each linebreak as <br>
//		(HOEDOWN_HTML_USE_XHTML)| // Render XHTML
		0);

const char *markdown2html(const char *input, int input_len) {
	static std::string output;
	hoedown_renderer *renderer = hoedown_html_renderer_new(flags, 0);
	hoedown_document *document = hoedown_document_new(renderer, extensions, 16);
	hoedown_buffer *html = hoedown_buffer_new(16);
	hoedown_document_render(document, html, reinterpret_cast<const uint8_t*>(input), input_len);
	output.assign(reinterpret_cast<char*>(html->data),html->size);
	hoedown_buffer_free(html);
	hoedown_document_free(document);
	hoedown_html_renderer_free(renderer);
	return output.c_str();
}
