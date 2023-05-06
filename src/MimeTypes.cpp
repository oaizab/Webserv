#include "MimeTypes.hpp"

void MimeTypes::init()
{
	data.insert(std::make_pair("html", "text/html"));
	data.insert(std::make_pair("htm", "text/html"));
	data.insert(std::make_pair("shtml", "text/html"));
	data.insert(std::make_pair("css", "text/css"));
	data.insert(std::make_pair("xml", "text/xml"));
	data.insert(std::make_pair("gif", "image/gif"));
	data.insert(std::make_pair("jpeg", "image/jpeg"));
	data.insert(std::make_pair("jpg", "image/jpeg"));
	data.insert(std::make_pair("js", "application/javascript"));
	data.insert(std::make_pair("txt", "text/plain"));
	data.insert(std::make_pair("png", "image/png"));
	data.insert(std::make_pair("svg", "image/svg+xml"));
	data.insert(std::make_pair("svgz", "image/svg+xml"));
	data.insert(std::make_pair("tif", "image/tiff"));
	data.insert(std::make_pair("tiff", "image/tiff"));
	data.insert(std::make_pair("webp", "image/webp"));
	data.insert(std::make_pair("ico", "image/x-icon"));
	data.insert(std::make_pair("jng", "image/x-jng"));
	data.insert(std::make_pair("bmp", "image/x-ms-bmp"));
	data.insert(std::make_pair("woff", "font/woff"));
	data.insert(std::make_pair("woff2", "font/woff2"));
	data.insert(std::make_pair("jar", "application/java-archive"));
	data.insert(std::make_pair("war", "application/java-archive"));
	data.insert(std::make_pair("ear", "application/java-archive"));
	data.insert(std::make_pair("json", "application/json"));
	data.insert(std::make_pair("doc", "application/msword"));
	data.insert(std::make_pair("pdf", "application/pdf"));
	data.insert(std::make_pair("ps", "application/postscript"));
	data.insert(std::make_pair("eps", "application/postscript"));
	data.insert(std::make_pair("ai", "application/postscript"));
	data.insert(std::make_pair("rtf", "application/rtf"));
	data.insert(std::make_pair("xls", "application/vnd.ms-excel"));
	data.insert(std::make_pair("ppt", "application/vnd.ms-powerpoint"));
	data.insert(std::make_pair("pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"));
	data.insert(std::make_pair("xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
	data.insert(std::make_pair("docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
	data.insert(std::make_pair("7z", "application/x-7z-compressed"));
	data.insert(std::make_pair("pl", "application/x-perl"));
	data.insert(std::make_pair("pm", "application/x-perl"));
	data.insert(std::make_pair("rar", "application/x-rar-compressed"));
	data.insert(std::make_pair("xhtml", "application/xhtml+xml"));
	data.insert(std::make_pair("xspf", "application/xspf+xml"));
	data.insert(std::make_pair("zip", "application/zip"));
	data.insert(std::make_pair("bin", "application/octet-stream"));
	data.insert(std::make_pair("exe", "application/octet-stream"));
	data.insert(std::make_pair("dll", "application/octet-stream"));
	data.insert(std::make_pair("deb", "application/octet-stream"));
	data.insert(std::make_pair("dmg", "application/octet-stream"));
	data.insert(std::make_pair("iso", "application/octet-stream"));
	data.insert(std::make_pair("img", "application/octet-stream"));
	data.insert(std::make_pair("msi", "application/octet-stream"));
	data.insert(std::make_pair("msp", "application/octet-stream"));
	data.insert(std::make_pair("msm", "application/octet-stream"));
	data.insert(std::make_pair("mid", "audio/midi"));
	data.insert(std::make_pair("midi", "audio/midi"));
	data.insert(std::make_pair("kar", "audio/midi"));
	data.insert(std::make_pair("mp3", "audio/mpeg"));
	data.insert(std::make_pair("ogg", "audio/ogg"));
	data.insert(std::make_pair("m4a", "audio/x-m4a"));
	data.insert(std::make_pair("3gpp", "video/3gpp"));
	data.insert(std::make_pair("3gp", "video/3gpp"));
	data.insert(std::make_pair("ts", "video/mp2t"));
	data.insert(std::make_pair("mp4", "video/mp4"));
	data.insert(std::make_pair("mpeg", "video/mpeg"));
	data.insert(std::make_pair("mpg", "video/mpeg"));
	data.insert(std::make_pair("mov", "video/quicktime"));
	data.insert(std::make_pair("webm", "video/webm"));
	data.insert(std::make_pair("flv", "video/x-flv"));
	data.insert(std::make_pair("m4v", "video/x-m4v"));
	data.insert(std::make_pair("wmv", "video/x-ms-wmv"));
	data.insert(std::make_pair("avi", "video/x-msvideo"));
}

std::string MimeTypes::getMimeType(const std::string &extension)
{
	if (data.empty())
	{
		init();
	}
	if (data.find(extension) == data.end())
	{
		return "application/octet-stream";
	}
	return data.at(extension);
}
