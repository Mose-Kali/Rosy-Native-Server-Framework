#include "pch.h"
#include "MosyHtmlSerializer.h"
#pragma warning(disable:4996)

MosyHtmlPackage MosyHtmlSerializer::AddString(std::wstring Context)
{
	MosyHtmlPackage Package;
	Package.Html.insert(Package.Html.begin(), Context.c_str(), Context.c_str() + Context.length());
	Package.Length = strlen(MosyString::WString2String(Context).c_str()) * sizeof(char);
	return Package;
}

MosyHtmlPackage MosyHtmlSerializer::AddString(std::string Context)
{
	MosyHtmlPackage Package;
	Package.Html.insert(Package.Html.begin(), Context.c_str(), Context.c_str() + Context.length());
	Package.Length = strlen(Context.c_str()) * sizeof(char);
	return Package;
}

MosyHtmlPackage MosyHtmlSerializer::AddFile(std::wstring FilePath)
{
	return AddFile(MosyString::WString2String(FilePath));
}

MosyHtmlPackage MosyHtmlSerializer::AddFile(std::string FilePath)
{
	MosyHtmlPackage Package;
	if (FilePath[0] == '/' || FilePath[0] == '.' || FilePath[0] == '\\')
	{
		wchar_t szExeFilePathFileName[MAX_PATH];
		GetModuleFileNameW(NULL, szExeFilePathFileName, MAX_PATH);
		std::wstring str = szExeFilePathFileName;
		int idx = str.find_last_of('\\');
		std::wstring sss = str.substr(0, idx);
		while (FilePath.find('/') != FilePath.npos)
		{
			int i = FilePath.find('/');
			FilePath.replace(i, 1, "\\");
		}
		FilePath.replace(0, 1, "");
		FilePath = MosyString::WString2String(sss) + FilePath;
		FilePath = FilePath.substr(0, FilePath.find_last_not_of(L' ') + 1);
	}
	std::wstring tp = MosyString::String2WString(FilePath.substr(FilePath.find_last_of('.') + 1, FilePath.length()));
	if (FileTypes.count(tp))
	{
		Package.type = FileTypes[tp];
	}
	else
	{
		tp = L"";
		Package.type = FileTypes[L""];
	}
	FILE *resource = fopen(FilePath.c_str(), "rb");
	if (resource == NULL)
	{
		MosyHtmlPackage p;
		p.Exit = false;
		const char* strCharA_ = "<HTML><TITLE>Not Found</TITLE>\r\n<BODY><h1 align='center'>404</h1><br/><h1 align='center'>NOT FOUND.</h1><br/><h1 align='center'>Mosy Server Framework</h1>\r\n</BODY></HTML>\r\n";
		p.Html.insert(p.Html.end(), strCharA_, strCharA_ + strlen(strCharA_));
		p.Length = p.Html.size();
		p.type = L"text/html";
		return p;
	}
	Package.Exit = true;
	fseek(resource, 0, SEEK_SET);
	fseek(resource, 0, SEEK_END);
	long flen = ftell(resource);
	Package.Length = flen;
	fseek(resource, 0, SEEK_SET);
	char send_buf[BUF_LENGTH * 1000];
	while (1)
	{
		memset(send_buf, 0, sizeof(send_buf));
		fread(send_buf, sizeof(char), BUF_LENGTH * 1000, resource);
		//Package.Html += MosyString::String2WString(send_buf);
		Package.Html.insert(Package.Html.end(), send_buf, send_buf + BUF_LENGTH * 1000);
		if (feof(resource))
			break;
	}
	//const wchar_t* ss = Package.Html.c_str();
	fclose(resource);
	return Package;
}

MosyHtmlSerializer::MosyHtmlSerializer()
{
	FileTypes.insert_or_assign(L"323", L"text/h323");
	FileTypes.insert_or_assign(L"biz", L"text/xml");
	FileTypes.insert_or_assign(L"cml", L"text/xml");
	FileTypes.insert_or_assign(L"asa", L"text/asa");
	FileTypes.insert_or_assign(L"asp", L"text/asp");
	FileTypes.insert_or_assign(L"css", L"text/css");
	FileTypes.insert_or_assign(L"csv", L"text/csv");
	FileTypes.insert_or_assign(L"dcd", L"text/xml");
	FileTypes.insert_or_assign(L"dtd", L"text/xml");
	FileTypes.insert_or_assign(L"ent", L"text/xml");
	FileTypes.insert_or_assign(L"fo", L"text/xml");
	FileTypes.insert_or_assign(L"htc", L"text/x-component");
	FileTypes.insert_or_assign(L"html", L"text/html");
	FileTypes.insert_or_assign(L"htx", L"text/html");
	FileTypes.insert_or_assign(L"htm", L"text/html");
	FileTypes.insert_or_assign(L"htt", L"text/webviewhtml");
	FileTypes.insert_or_assign(L"jsp", L"text/html");
	FileTypes.insert_or_assign(L"math", L"text/xml");
	FileTypes.insert_or_assign(L"mml", L"text/xml");
	FileTypes.insert_or_assign(L"mtx", L"text/xml");
	FileTypes.insert_or_assign(L"plg", L"text/html");
	FileTypes.insert_or_assign(L"rdf", L"text/xml");
	FileTypes.insert_or_assign(L"rt", L"text/vnd.rn-realtext");
	FileTypes.insert_or_assign(L"sol", L"text/plain");
	FileTypes.insert_or_assign(L"spp", L"text/xml");
	FileTypes.insert_or_assign(L"stm", L"text/html");
	FileTypes.insert_or_assign(L"svg", L"image/svg+xml");
	FileTypes.insert_or_assign(L"tld", L"text/xml");
	FileTypes.insert_or_assign(L"txt", L"text/plain");
	FileTypes.insert_or_assign(L"uls", L"text/iuls");
	FileTypes.insert_or_assign(L"vml", L"text/xml");
	FileTypes.insert_or_assign(L"tsd", L"text/xml");
	FileTypes.insert_or_assign(L"vcf", L"text/x-vcard");
	FileTypes.insert_or_assign(L"vxml", L"text/xml");
	FileTypes.insert_or_assign(L"wml", L"text/vnd.wap.wml");
	FileTypes.insert_or_assign(L"wsdl", L"text/xml");
	FileTypes.insert_or_assign(L"wsc", L"text/scriptlet");
	FileTypes.insert_or_assign(L"xdr", L"text/xml");
	FileTypes.insert_or_assign(L"xql", L"text/xml");
	FileTypes.insert_or_assign(L"xsd", L"text/xml");
	FileTypes.insert_or_assign(L"xslt", L"text/xml");
	FileTypes.insert_or_assign(L"xml", L"text/xml");
	FileTypes.insert_or_assign(L"xq", L"text/xml");
	FileTypes.insert_or_assign(L"xquery", L"text/xml");
	FileTypes.insert_or_assign(L"xsl", L"text/xml");
	FileTypes.insert_or_assign(L"xhtml", L"text/html");
	FileTypes.insert_or_assign(L"odc", L"text/x-ms-odc");
	FileTypes.insert_or_assign(L"r3t", L"text/vnd.rn-realtext3d");
	FileTypes.insert_or_assign(L"sor", L"text/plain");
	FileTypes.insert_or_assign(L"acp", L"audio/x-mei-aac");
	FileTypes.insert_or_assign(L"aif", L"audio/aiff");
	FileTypes.insert_or_assign(L"aiff", L"audio/aiff");
	FileTypes.insert_or_assign(L"aifc", L"audio/aiff");
	FileTypes.insert_or_assign(L"au", L"audio/basic");
	FileTypes.insert_or_assign(L"la1", L"audio/x-liquid-file");
	FileTypes.insert_or_assign(L"lavs", L"audio/x-liquid-secure");
	FileTypes.insert_or_assign(L"lmsff", L"audio/x-la-lms");
	FileTypes.insert_or_assign(L"m3u", L"audio/mpegurl");
	FileTypes.insert_or_assign(L"midi", L"audio/mid");
	FileTypes.insert_or_assign(L"mid", L"audio/mid");
	FileTypes.insert_or_assign(L"mp2", L"audio/mpeg");
	FileTypes.insert_or_assign(L"mp3", L"audio/mpeg");
	FileTypes.insert_or_assign(L"mp4", L"audio/mpeg");
	FileTypes.insert_or_assign(L"mnd", L"audio/x-musicnet-download");
	FileTypes.insert_or_assign(L"mp1", L"audio/mp1");
	FileTypes.insert_or_assign(L"mns", L"audio/x-musicnet-stream");
	FileTypes.insert_or_assign(L"mpga", L"audio/rn-mpeg");
	FileTypes.insert_or_assign(L"pls", L"audio/scpls");
	FileTypes.insert_or_assign(L"ram", L"audio/x-pn-realaudio");
	FileTypes.insert_or_assign(L"rmi", L"audio/mid");
	FileTypes.insert_or_assign(L"rmm", L"audio/x-pn-realaudio");
	FileTypes.insert_or_assign(L"snd", L"audio/basic");
	FileTypes.insert_or_assign(L"wav", L"audio/wav");
	FileTypes.insert_or_assign(L"wax", L"audio/x-ms-wax");
	FileTypes.insert_or_assign(L"wma", L"audio/x-ms-wma");
	FileTypes.insert_or_assign(L"", L"application/x-");
	FileTypes.insert_or_assign(L"*", L"application/octet-stream");
	FileTypes.insert_or_assign(L"pdf", L"application/pdf");
	FileTypes.insert_or_assign(L"ai", L"application/postscript");
	FileTypes.insert_or_assign(L"xml", L"application/atom+xml");
	FileTypes.insert_or_assign(L"js", L"application/ecmascript");
	FileTypes.insert_or_assign(L"edi", L"application/EDI-X12");
	FileTypes.insert_or_assign(L"edi", L"application/EDIFACT");
	FileTypes.insert_or_assign(L"json", L"application/json");
	FileTypes.insert_or_assign(L"js", L"application/javascript");
	FileTypes.insert_or_assign(L"ogg", L"application/ogg");
	FileTypes.insert_or_assign(L"rdf", L"application/rdf+xml");
	FileTypes.insert_or_assign(L"xml", L"application/rss+xml");
	FileTypes.insert_or_assign(L"xml", L"application/soap+xml");
	FileTypes.insert_or_assign(L"woff", L"application/font-woff");
	FileTypes.insert_or_assign(L"xhtml", L"application/xhtml+xml");
	FileTypes.insert_or_assign(L"xml", L"application/xml");
	FileTypes.insert_or_assign(L"dtd", L"application/xml-dtd");
	FileTypes.insert_or_assign(L"xml", L"application/xop+xml");
	FileTypes.insert_or_assign(L"zip", L"application/zip");
	FileTypes.insert_or_assign(L"gzip", L"application/gzip");
	FileTypes.insert_or_assign(L"xls", L"application/x-xls");
	FileTypes.insert_or_assign(L"001", L"application/x-001");
	FileTypes.insert_or_assign(L"301", L"application/x-301");
	FileTypes.insert_or_assign(L"906", L"application/x-906");
	FileTypes.insert_or_assign(L"a11", L"application/x-a11");
	FileTypes.insert_or_assign(L"awf", L"application/vnd.adobe.workflow");
	FileTypes.insert_or_assign(L"bmp", L"application/x-bmp");
	FileTypes.insert_or_assign(L"c4t", L"application/x-c4t");
	FileTypes.insert_or_assign(L"cal", L"application/x-cals");
	FileTypes.insert_or_assign(L"cdf", L"application/x-netcdf");
	FileTypes.insert_or_assign(L"cel", L"application/x-cel");
	FileTypes.insert_or_assign(L"cg4", L"application/x-g4");
	FileTypes.insert_or_assign(L"cit", L"application/x-cit");
	FileTypes.insert_or_assign(L"bot", L"application/x-bot");
	FileTypes.insert_or_assign(L"c90", L"application/x-c90");
	FileTypes.insert_or_assign(L"cat", L"application/vnd.ms-pki.seccat");
	FileTypes.insert_or_assign(L"cdr", L"application/x-cdr");
	FileTypes.insert_or_assign(L"cer", L"application/x-x509-ca-cert");
	FileTypes.insert_or_assign(L"cgm", L"application/x-cgm");
	FileTypes.insert_or_assign(L"cmx", L"application/x-cmx");
	FileTypes.insert_or_assign(L"crl", L"application/pkix-crl");
	FileTypes.insert_or_assign(L"csi", L"application/x-csi");
	FileTypes.insert_or_assign(L"cut", L"application/x-cut");
	FileTypes.insert_or_assign(L"dbm", L"application/x-dbm");
	FileTypes.insert_or_assign(L"cmp", L"application/x-cmp");
	FileTypes.insert_or_assign(L"cot", L"application/x-cot");
	FileTypes.insert_or_assign(L"crt", L"application/x-x509-ca-cert");
	FileTypes.insert_or_assign(L"dbf", L"application/x-dbf");
	FileTypes.insert_or_assign(L"dbx", L"application/x-dbx");
	FileTypes.insert_or_assign(L"dcx", L"application/x-dcx");
	FileTypes.insert_or_assign(L"dgn", L"application/x-dgn");
	FileTypes.insert_or_assign(L"dll", L"application/x-msdownload");
	FileTypes.insert_or_assign(L"dot", L"application/msword");
	FileTypes.insert_or_assign(L"der", L"application/x-x509-ca-cert");
	FileTypes.insert_or_assign(L"dib", L"application/x-dib");
	FileTypes.insert_or_assign(L"doc", L"application/msword");
	FileTypes.insert_or_assign(L"drw", L"application/x-drw");
	FileTypes.insert_or_assign(L"dwf", L"application/x-dwf");
	FileTypes.insert_or_assign(L"dxb", L"application/x-dxb");
	FileTypes.insert_or_assign(L"edn", L"application/vnd.adobe.edn");
	FileTypes.insert_or_assign(L"dwg", L"application/x-dwg");
	FileTypes.insert_or_assign(L"dxf", L"application/x-dxf");
	FileTypes.insert_or_assign(L"emf", L"application/x-emf");
	FileTypes.insert_or_assign(L"epi", L"application/x-epi");
	FileTypes.insert_or_assign(L"eps", L"application/postscript");
	FileTypes.insert_or_assign(L"exe", L"application/x-msdownload");
	FileTypes.insert_or_assign(L"fdf", L"application/vnd.fdf");
	FileTypes.insert_or_assign(L"eps", L"application/x-ps");
	FileTypes.insert_or_assign(L"etd", L"application/x-ebx");
	FileTypes.insert_or_assign(L"fif", L"application/fractals");
	FileTypes.insert_or_assign(L"frm", L"application/x-frm");
	FileTypes.insert_or_assign(L"gbr", L"application/x-gbr");
	FileTypes.insert_or_assign(L"g4", L"application/x-g4");
	FileTypes.insert_or_assign(L"gl2", L"application/x-gl2");
	FileTypes.insert_or_assign(L"hgl", L"application/x-hgl");
	FileTypes.insert_or_assign(L"hpg", L"application/x-hpgl");
	FileTypes.insert_or_assign(L"hqx", L"application/mac-binhex40");
	FileTypes.insert_or_assign(L"hta", L"application/hta");
	FileTypes.insert_or_assign(L"gp4", L"application/x-gp4");
	FileTypes.insert_or_assign(L"hmr", L"application/x-hmr");
	FileTypes.insert_or_assign(L"hpl", L"application/x-hpl");
	FileTypes.insert_or_assign(L"hrf", L"application/x-hrf");
	FileTypes.insert_or_assign(L"icb", L"application/x-icb");
	FileTypes.insert_or_assign(L"ico", L"application/x-ico");
	FileTypes.insert_or_assign(L"ig4", L"application/x-g4");
	FileTypes.insert_or_assign(L"iii", L"application/x-iphone");
	FileTypes.insert_or_assign(L"ins", L"application/x-internet-signup");
	FileTypes.insert_or_assign(L"iff", L"application/x-iff");
	FileTypes.insert_or_assign(L"igs", L"application/x-igs");
	FileTypes.insert_or_assign(L"img", L"application/x-img");
	FileTypes.insert_or_assign(L"isp", L"application/x-internet-signup");
	FileTypes.insert_or_assign(L"jpe", L"application/x-jpe");
	FileTypes.insert_or_assign(L"js", L"text/js");
	FileTypes.insert_or_assign(L"jpg", L"application/x-jpg");
	FileTypes.insert_or_assign(L"lar", L"application/x-laplayer-reg");
	FileTypes.insert_or_assign(L"latex", L"application/x-latex");
	FileTypes.insert_or_assign(L"lbm", L"application/x-lbm");
	FileTypes.insert_or_assign(L"ls", L"application/x-javascript");
	FileTypes.insert_or_assign(L"ltr", L"application/x-ltr");
	FileTypes.insert_or_assign(L"man", L"application/x-troff-man");
	FileTypes.insert_or_assign(L"mdb", L"application/msaccess");
	FileTypes.insert_or_assign(L"mac", L"application/x-mac");
	FileTypes.insert_or_assign(L"mdb", L"application/x-mdb");
	FileTypes.insert_or_assign(L"mfp", L"application/x-shockwave-flash");
	FileTypes.insert_or_assign(L"mi", L"application/x-mi");
	FileTypes.insert_or_assign(L"mil", L"application/x-mil");
	FileTypes.insert_or_assign(L"mocha", L"application/x-javascript");
	FileTypes.insert_or_assign(L"mpd", L"application/vnd.ms-project");
	FileTypes.insert_or_assign(L"mpp", L"application/vnd.ms-project");
	FileTypes.insert_or_assign(L"mpt", L"application/vnd.ms-project");
	FileTypes.insert_or_assign(L"mpw", L"application/vnd.ms-project");
	FileTypes.insert_or_assign(L"mpx", L"application/vnd.ms-project");
	FileTypes.insert_or_assign(L"mxp", L"application/x-mmxp");
	FileTypes.insert_or_assign(L"nrf", L"application/x-nrf");
	FileTypes.insert_or_assign(L"out", L"application/x-out");
	FileTypes.insert_or_assign(L"p12", L"application/x-pkcs12");
	FileTypes.insert_or_assign(L"p7c", L"application/pkcs7-mime");
	FileTypes.insert_or_assign(L"p7r", L"application/x-pkcs7-certreqresp");
	FileTypes.insert_or_assign(L"pc5", L"application/x-pc5");
	FileTypes.insert_or_assign(L"pcl", L"application/x-pcl");
	FileTypes.insert_or_assign(L"pdx", L"application/vnd.adobe.pdx");
	FileTypes.insert_or_assign(L"pgl", L"application/x-pgl");
	FileTypes.insert_or_assign(L"pko", L"application/vnd.ms-pki.pko");
	FileTypes.insert_or_assign(L"p10", L"application/pkcs10");
	FileTypes.insert_or_assign(L"p7b", L"application/x-pkcs7-certificates");
	FileTypes.insert_or_assign(L"p7m", L"application/pkcs7-mime");
	FileTypes.insert_or_assign(L"p7s", L"application/pkcs7-signature");
	FileTypes.insert_or_assign(L"pci", L"application/x-pci");
	FileTypes.insert_or_assign(L"pcx", L"application/x-pcx");
	FileTypes.insert_or_assign(L"pdf", L"application/pdf");
	FileTypes.insert_or_assign(L"pfx", L"application/x-pkcs12");
	FileTypes.insert_or_assign(L"pic", L"application/x-pic");
	FileTypes.insert_or_assign(L"pl", L"application/x-perl");
	FileTypes.insert_or_assign(L"plt", L"application/x-plt");
	FileTypes.insert_or_assign(L"png", L"application/x-png");
	FileTypes.insert_or_assign(L"ppa", L"application/vnd.ms-powerpoint");
	FileTypes.insert_or_assign(L"pps", L"application/vnd.ms-powerpoint");
	FileTypes.insert_or_assign(L"ppt", L"application/x-ppt");
	FileTypes.insert_or_assign(L"prf", L"application/pics-rules");
	FileTypes.insert_or_assign(L"prt", L"application/x-prt");
	FileTypes.insert_or_assign(L"ps", L"application/postscript");
	FileTypes.insert_or_assign(L"pwz", L"application/vnd.ms-powerpoint");
	FileTypes.insert_or_assign(L"ra", L"audio/vnd.rn-realaudio");
	FileTypes.insert_or_assign(L"ras", L"application/x-ras");
	FileTypes.insert_or_assign(L"pot", L"application/vnd.ms-powerpoint");
	FileTypes.insert_or_assign(L"ppm", L"application/x-ppm");
	FileTypes.insert_or_assign(L"ppt", L"application/vnd.ms-powerpoint");
	FileTypes.insert_or_assign(L"pr", L"application/x-pr");
	FileTypes.insert_or_assign(L"prn", L"application/x-prn");
	FileTypes.insert_or_assign(L"ps", L"application/x-ps");
	FileTypes.insert_or_assign(L"ptn", L"application/x-ptn");
	FileTypes.insert_or_assign(L"red", L"application/x-red");
	FileTypes.insert_or_assign(L"rjs", L"application/vnd.rn-realsystem-rjs");
	FileTypes.insert_or_assign(L"rlc", L"application/x-rlc");
	FileTypes.insert_or_assign(L"rm", L"application/vnd.rn-realmedia");
	FileTypes.insert_or_assign(L"rat", L"application/rat-file");
	FileTypes.insert_or_assign(L"rec", L"application/vnd.rn-recording");
	FileTypes.insert_or_assign(L"rgb", L"application/x-rgb");
	FileTypes.insert_or_assign(L"rjt", L"application/vnd.rn-realsystem-rjt");
	FileTypes.insert_or_assign(L"rle", L"application/x-rle");
	FileTypes.insert_or_assign(L"rmf", L"application/vnd.adobe.rmf");
	FileTypes.insert_or_assign(L"rmj", L"application/vnd.rn-realsystem-rmj");
	FileTypes.insert_or_assign(L"rmp", L"application/vnd.rn-rn_music_package");
	FileTypes.insert_or_assign(L"rmvb", L"application/vnd.rn-realmedia-vbr");
	FileTypes.insert_or_assign(L"rnx", L"application/vnd.rn-realplayer");
	FileTypes.insert_or_assign(L"rpm", L"audio/x-pn-realaudio-plugin");
	FileTypes.insert_or_assign(L"rms", L"application/vnd.rn-realmedia-secure");
	FileTypes.insert_or_assign(L"rmx", L"application/vnd.rn-realsystem-rmx");
	FileTypes.insert_or_assign(L"rsml", L"application/vnd.rn-rsml");
	FileTypes.insert_or_assign(L"rtf", L"application/msword");
	FileTypes.insert_or_assign(L"rv", L"video/vnd.rn-realvideo");
	FileTypes.insert_or_assign(L"sat", L"application/x-sat");
	FileTypes.insert_or_assign(L"sdw", L"application/x-sdw");
	FileTypes.insert_or_assign(L"slb", L"application/x-slb");
	FileTypes.insert_or_assign(L"rtf", L"application/x-rtf");
	FileTypes.insert_or_assign(L"sam", L"application/x-sam");
	FileTypes.insert_or_assign(L"sdp", L"application/sdp");
	FileTypes.insert_or_assign(L"sit", L"application/x-stuffit");
	FileTypes.insert_or_assign(L"sld", L"application/x-sld");
	FileTypes.insert_or_assign(L"smi", L"application/smil");
	FileTypes.insert_or_assign(L"smk", L"application/x-smk");
	FileTypes.insert_or_assign(L"smil", L"application/smil");
	FileTypes.insert_or_assign(L"spc", L"application/x-pkcs7-certificates");
	FileTypes.insert_or_assign(L"spl", L"application/futuresplash");
	FileTypes.insert_or_assign(L"ssm", L"application/streamingmedia");
	FileTypes.insert_or_assign(L"stl", L"application/vnd.ms-pki.stl");
	FileTypes.insert_or_assign(L"sst", L"application/vnd.ms-pki.certstore");
	FileTypes.insert_or_assign(L"tdf", L"application/x-tdf");
	FileTypes.insert_or_assign(L"tga", L"application/x-tga");
	FileTypes.insert_or_assign(L"sty", L"application/x-sty");
	FileTypes.insert_or_assign(L"swf", L"application/x-shockwave-flash");
	FileTypes.insert_or_assign(L"tg4", L"application/x-tg4");
	FileTypes.insert_or_assign(L"tif", L"application/x-tif");
	FileTypes.insert_or_assign(L"vdx", L"application/vnd.visio");
	FileTypes.insert_or_assign(L"vpg", L"application/x-vpeg005");
	FileTypes.insert_or_assign(L"vsd", L"application/x-vsd");
	FileTypes.insert_or_assign(L"vst", L"application/vnd.visio");
	FileTypes.insert_or_assign(L"vsw", L"application/vnd.visio");
	FileTypes.insert_or_assign(L"vtx", L"application/vnd.visio");
	FileTypes.insert_or_assign(L"torrent", L"application/x-bittorrent");
	FileTypes.insert_or_assign(L"vda", L"application/x-vda");
	FileTypes.insert_or_assign(L"vsd", L"application/vnd.visio");
	FileTypes.insert_or_assign(L"vss", L"application/vnd.visio");
	FileTypes.insert_or_assign(L"vst", L"application/x-vst");
	FileTypes.insert_or_assign(L"vsx", L"application/vnd.visio");
	FileTypes.insert_or_assign(L"wb1", L"application/x-wb1");
	FileTypes.insert_or_assign(L"wb3", L"application/x-wb3");
	FileTypes.insert_or_assign(L"wiz", L"application/msword");
	FileTypes.insert_or_assign(L"wk4", L"application/x-wk4");
	FileTypes.insert_or_assign(L"wks", L"application/x-wks");
	FileTypes.insert_or_assign(L"wb2", L"application/x-wb2");
	FileTypes.insert_or_assign(L"wk3", L"application/x-wk3");
	FileTypes.insert_or_assign(L"wkq", L"application/x-wkq");
	FileTypes.insert_or_assign(L"wmf", L"application/x-wmf");
	FileTypes.insert_or_assign(L"wmd", L"application/x-ms-wmd");
	FileTypes.insert_or_assign(L"wp6", L"application/x-wp6");
	FileTypes.insert_or_assign(L"wpg", L"application/x-wpg");
	FileTypes.insert_or_assign(L"wq1", L"application/x-wq1");
	FileTypes.insert_or_assign(L"wri", L"application/x-wri");
	FileTypes.insert_or_assign(L"ws", L"application/x-ws");
	FileTypes.insert_or_assign(L"wmz", L"application/x-ms-wmz");
	FileTypes.insert_or_assign(L"wpd", L"application/x-wpd");
	FileTypes.insert_or_assign(L"wpl", L"application/vnd.ms-wpl");
	FileTypes.insert_or_assign(L"wr1", L"application/x-wr1");
	FileTypes.insert_or_assign(L"wrk", L"application/x-wrk");
	FileTypes.insert_or_assign(L"ws2", L"application/x-ws");
	FileTypes.insert_or_assign(L"xdp", L"application/vnd.adobe.xdp");
	FileTypes.insert_or_assign(L"xfd", L"application/vnd.adobe.xfd");
	FileTypes.insert_or_assign(L"xfdf", L"application/vnd.adobe.xfdf");
	FileTypes.insert_or_assign(L"xls", L"application/vnd.ms-excel");
	FileTypes.insert_or_assign(L"xwd", L"application/x-xwd");
	FileTypes.insert_or_assign(L"sis", L"application/vnd.symbian.install");
	FileTypes.insert_or_assign(L"x_t", L"application/x-x_t");
	FileTypes.insert_or_assign(L"apk", L"application/vnd.android.package-archive");
	FileTypes.insert_or_assign(L"x_b", L"application/x-x_b");
	FileTypes.insert_or_assign(L"sisx", L"application/vnd.symbian.install");
	FileTypes.insert_or_assign(L"ipa", L"application/vnd.iphone");
	FileTypes.insert_or_assign(L"xap", L"application/x-silverlight-app");
	FileTypes.insert_or_assign(L"xlw", L"application/x-xlw");
	FileTypes.insert_or_assign(L"xpl", L"audio/scpls");
	FileTypes.insert_or_assign(L"anv", L"application/x-anv");
	FileTypes.insert_or_assign(L"uin", L"application/x-icq");
	FileTypes.insert_or_assign(L"asf", L"video/x-ms-asf");
	FileTypes.insert_or_assign(L"asx", L"video/x-ms-asf");
	FileTypes.insert_or_assign(L"avi", L"video/avi");
	FileTypes.insert_or_assign(L"IVF", L"video/x-ivf");
	FileTypes.insert_or_assign(L"m1v", L"video/x-mpeg");
	FileTypes.insert_or_assign(L"m2v", L"video/x-mpeg");
	FileTypes.insert_or_assign(L"m4e", L"video/mpeg4");
	FileTypes.insert_or_assign(L"movie", L"video/x-sgi-movie");
	FileTypes.insert_or_assign(L"mp2v", L"video/mpeg");
	FileTypes.insert_or_assign(L"mp4", L"audio/mpeg");
	FileTypes.insert_or_assign(L"mpa", L"video/x-mpg");
	FileTypes.insert_or_assign(L"mpe", L"video/x-mpeg");
	FileTypes.insert_or_assign(L"mpg", L"video/mpg");
	FileTypes.insert_or_assign(L"mpeg", L"video/mpg");
	FileTypes.insert_or_assign(L"mps", L"video/x-mpeg");
	FileTypes.insert_or_assign(L"mpv", L"video/mpg");
	FileTypes.insert_or_assign(L"mpv2", L"video/mpeg");
	FileTypes.insert_or_assign(L"wm", L"video/x-ms-wm");
	FileTypes.insert_or_assign(L"wmv", L"video/x-ms-wmv");
	FileTypes.insert_or_assign(L"wmx", L"video/x-ms-wmx");
	FileTypes.insert_or_assign(L"wvx", L"video/x-ms-wvx");
	FileTypes.insert_or_assign(L"tif", L"image/tiff");
	FileTypes.insert_or_assign(L"fax", L"image/fax");
	FileTypes.insert_or_assign(L"gif", L"image/gif");
	FileTypes.insert_or_assign(L"ico", L"image/x-icon");
	FileTypes.insert_or_assign(L"jfif", L"image/jpeg");
	FileTypes.insert_or_assign(L"jpe", L"image/jpeg");
	FileTypes.insert_or_assign(L"jpeg", L"image/jpeg");
	FileTypes.insert_or_assign(L"jpg", L"image/jpeg");
	FileTypes.insert_or_assign(L"net", L"image/pnetvue");
	FileTypes.insert_or_assign(L"png", L"image/png");
	FileTypes.insert_or_assign(L"rp", L"image/vnd.rn-realpix");
	FileTypes.insert_or_assign(L"tif", L"image/tiff");
	FileTypes.insert_or_assign(L"tiff", L"image/tiff");
	FileTypes.insert_or_assign(L"wbmp", L"image/vnd.wap.wbmp");
	FileTypes.insert_or_assign(L"eml", L"message/rfc822");
	FileTypes.insert_or_assign(L"mht", L"message/rfc822");
	FileTypes.insert_or_assign(L"mhtml", L"message/rfc822");
	FileTypes.insert_or_assign(L"nws", L"message/rfc822");
	FileTypes.insert_or_assign(L"907", L"drawing/907");
	FileTypes.insert_or_assign(L"slk", L"drawing/x-slk");
	FileTypes.insert_or_assign(L"top", L"drawing/x-top");
	FileTypes.insert_or_assign(L"class", L"java/*");
	FileTypes.insert_or_assign(L"java", L"java/*");
	FileTypes.insert_or_assign(L"dwf", L"Model/vnd.dwf");
}

MosyHtmlSerializer::~MosyHtmlSerializer()
{
	FileTypes.clear();
}
