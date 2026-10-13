#include "app_settings_service.h"
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#endif

#ifndef _WIN32
#include <sys/stat.h>
#endif

namespace {
std::string ExtractValue(const std::string& json, const std::string& key) {
	std::string pattern = "\"" + key + "\"";
	size_t pos = json.find(pattern);
	if (pos == std::string::npos) { return ""; }

	size_t colon = json.find(':', pos + pattern.size());
	if (colon == std::string::npos) { return ""; }

	size_t firstQuote = json.find('"', colon);
	if (firstQuote == std::string::npos) { return ""; }

	size_t secondQuote = json.find('"', firstQuote + 1);
	if (secondQuote == std::string::npos) { return ""; }

	return json.substr(firstQuote + 1, secondQuote - firstQuote - 1);
}

std::string EscapeJson(const std::string& value) {
	std::string out;
	for (char c : value) {
		if (c == '\\' || c == '"') {
			out += '\\';
		}
		out += c;
	}
	return out;
}

std::string ToJson(const AppSettings& settings) {
	return std::string("{\n    \"calendarApiKey\": \"")
		+ EscapeJson(settings.calendarApiKey)
		+ "\",\n    \"aiApiKey\": \""
		+ EscapeJson(settings.aiApiKey)
		+ "\",\n    \"theme\": \""
		+ EscapeJson(settings.theme)
		+ "\"\n}\n";
}

AppSettings FromJson(const std::string& json) {
	AppSettings settings;
	settings.calendarApiKey = ExtractValue(json, "calendarApiKey");
	settings.aiApiKey = ExtractValue(json, "aiApiKey");
	settings.theme = ExtractValue(json, "theme");
	return settings;
}

wxString SettingsDir() {
	return wxStandardPaths::Get().GetUserDataDir();
}

wxString SettingsPath() {
	wxFileName path;
	path.AssignDir(SettingsDir());
#ifdef _WIN32
	path.SetFullName("settings.bin");
#else
	path.SetFullName("settings.json");
#endif
	return path.GetFullPath();
}

bool EnsureSettingsDir() {
	wxFileName dir;
	dir.AssignDir(SettingsDir());
	if (dir.DirExists()) {
		return true;
	}
	return dir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
}

bool WriteBytes(const wxString& path, const std::string& bytes) {
	wxFile file;
	if (!file.Create(path, true)) {
		return false;
	}
	if (bytes.empty()) {
		return true;
	}
	return file.Write(bytes.data(), bytes.size()) == bytes.size();
}

std::string ReadBytes(const wxString& path) {
	if (!wxFile::Exists(path)) {
		return "";
	}
	wxFile file(path, wxFile::read);
	if (!file.IsOpened()) {
		return "";
	}
	const wxFileOffset length = file.Length();
	if (length <= 0) {
		return "";
	}
	std::string data(static_cast<size_t>(length), '\0');
	const size_t read = file.Read(&data[0], data.size());
	if (read != data.size()) {
		return "";
	}
	return data;
}

#ifdef _WIN32
std::string Protect(const std::string& plaintext) {
	DATA_BLOB input;
	input.pbData = reinterpret_cast<BYTE*>(const_cast<char*>(plaintext.data()));
	input.cbData = static_cast<DWORD>(plaintext.size());
	DATA_BLOB output{};
	if (!CryptProtectData(&input, L"Merken", nullptr, nullptr, nullptr, 0, &output)) {
		return "";
	}
	std::string blob(reinterpret_cast<char*>(output.pbData), output.cbData);
	LocalFree(output.pbData);
	return blob;
}

std::string Unprotect(const std::string& blob) {
	if (blob.empty()) {
		return "";
	}
	DATA_BLOB input;
	input.pbData = reinterpret_cast<BYTE*>(const_cast<char*>(blob.data()));
	input.cbData = static_cast<DWORD>(blob.size());
	DATA_BLOB output{};
	if (!CryptUnprotectData(&input, nullptr, nullptr, nullptr, nullptr, 0, &output)) {
		return "";
	}
	std::string text(reinterpret_cast<char*>(output.pbData), output.cbData);
	LocalFree(output.pbData);
	return text;
}
#endif
}

AppSettings AppSettingsService::GetSettings() {
	const std::string stored = ReadBytes(SettingsPath());
	if (stored.empty()) {
		return AppSettings{};
	}
#ifdef _WIN32
	const std::string json = Unprotect(stored);
	if (json.empty()) {
		return AppSettings{};
	}
	return FromJson(json);
#else
	return FromJson(stored);
#endif
}

bool AppSettingsService::SaveSettings(const AppSettings& settings) {
	if (!EnsureSettingsDir()) {
		return false;
	}
	const std::string json = ToJson(settings);
#ifdef _WIN32
	const std::string blob = Protect(json);
	if (blob.empty()) {
		return false;
	}
	return WriteBytes(SettingsPath(), blob);
#else
	if (!WriteBytes(SettingsPath(), json)) {
		return false;
	}
	return chmod(SettingsPath().ToUTF8().data(), S_IRUSR | S_IWUSR) == 0;
#endif
}
