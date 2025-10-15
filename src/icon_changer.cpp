////////////////////////////////////////////////////////////////////////////////
// This is free and unencumbered software released into the public domain.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to https://unlicense.org
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// HEADER FILE INCLUDES
////////////////////////////////////////////////////////////////////////////////

#include "icon_changer.hpp"

#include <cassert>
#include <filesystem>
#include <gdiplus.h>
#include <print>
#include <stdexcept>
#include <vector>
#include <windows.h>

#include "ansi_color_codes.hpp"
#include "icon.hpp"
#pragma comment(lib, "Gdiplus.lib")
using namespace Gdiplus;

////////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

///
/// \brief Validates the number of command-line arguments.
/// \details If the argument count is incorrect, usage information is printed
/// and an exception is thrown for too few arguments.
/// \param argument_count: The number of command-line arguments passed.
/// \param program_path: The path of the executing program (used in usage message).
///
static void validate_argument_count(std::int32_t     argument_count,
                                    std::string_view program_path);

///
/// \brief Entry point to initiate the icon replacement in an executable.
/// \details Verifies files existence and forwards the call to the secure
/// version.
/// \param icon_path: The path to the `.ico` file.
/// \param executable_path: The path to the target `.exe` file.
///
static void change_icon(std::string_view icon_path,
                        std::string_view executable_path);

///
/// \brief Secure version of icon replacement with rollback on failure.
/// \details Opens the executable's resources, sets the icon images and header,
/// and commits the changes.
/// \param icon_path: The path to the `.ico` file.
/// \param executable_path: The path to the target `.exe` file.
///
static void change_icon_s(std::string_view icon_path,
                          std::string_view executable_path);

///
/// \brief Adds the individual icon image resources to the executable.
/// \details Iterates over all images and adds them with appropriate resource IDs.
/// \param exe_resource: Handle to the open resource section of the executable.
/// \param icon: The parsed icon object containing image data.
///
static void set_images(void* exe_resource,
                       icon& icon);

///
/// \brief Adds the group icon header (NEWHEADER + RESDIR) to the executable.
/// \param exe_resource: Handle to the open resource section of the executable.
/// \param icon: The parsed icon object containing the group icon header.
///
static void set_icon_header(void*       exe_resource,
                            const icon& icon);

///
/// \brief Detects the type of image file based on its extension.
/// \param path: Path to the image file.
/// \return One of: "ico", "png", "bmp", "jpg", "jpeg", or empty string if unknown.
///
static std::string get_image_type(const std::string_view path);

///
/// \brief Checks if the given image has a supported format.
/// \details Converts the file extension to lowercase and verifies that it is
/// one of the allowed types (.ico, .png, .bmp). Throws an exception if the
/// format is unsupported.
/// \param image: Path to the image file to validate.
///
static void verify_supported_image(const std::string_view& image);

///
/// \brief Prepares the icon for embedding into the executable.
/// \details Validates the image format and (in the future) converts non-.ico
/// formats (e.g. .png, .bmp) into a temporary .ico file.
/// \param icon_path: The path to the input image file.
/// \return Path to a valid .ico file ready for use.
///
static std::string prepare_icon(const std::string_view icon_path);

////////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

static void verify_supported_image(const std::string_view& image)
{
	namespace fs    = std::filesystem;
	std::string ext = fs::path(image).extension().string();

	// convert to lowercase
	std::transform(ext.begin(),
	               ext.end(),
	               ext.begin(),
	               [](unsigned char c)
	               {
		               return std::tolower(c);
	               });
	if (ext != ".ico" && ext != ".png" && ext != ".bmp")
	{
		throw std::invalid_argument{ std::format("Unsupported image type '{}'. Allowed formats: .ico, .png, .bmp", ext) };
	}
}

static std::string get_image_type(const std::string_view path)
{
	namespace fs    = std::filesystem;
	std::string ext = fs::path(path).extension().string();

	//Convert to lowercase for safety
	std::transform(ext.begin(),
	               ext.end(),
	               ext.begin(),
	               [](unsigned char c)
	               {
		               return std::tolower(c);
	               });

	if (ext == ".ico")
	{
		return "ico";
	}
	if (ext == ".png")
	{
		return "png";
	}
	if (ext == ".bmp")
	{
		return "bmp";
	}

	return "";
}

static std::string prepare_icon(const std::string_view icon_path)
{
	namespace fs = std::filesystem;
	verify_supported_image(icon_path);

	const auto type = get_image_type(icon_path);
	if (type == "ico")
	{
		//Already a valid .ico file - procceded as is
		return std::string(icon_path);
	}
	else if (type == "bmp" || type == "png")
	{
		//Initialize GDI+
		GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR           gdiplusToken;

		if (GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr) != Ok)
		{
			throw std::runtime_error("Failed to initialize GDI+");
		}

		std::string output_path = (fs::path(icon_path).parent_path() / "converted.ico").string();

		//load source image
		std::wstring winput(icon_path.begin(), icon_path.end());
		Bitmap*      bmp = Bitmap::FromFile(winput.c_str());

		if (!bmp || bmp->GetLastStatus() != Ok)
		{
			GdiplusShutdown(gdiplusToken);
			throw std::runtime_error(std::format("Failed to load image {}", icon_path));
		}

		//Find ICO encoder
		CLSID clsidEncoder;
		UINT  numEncoders = 0, size = 0;
		GetImageEncodersSize(&numEncoders, &size);
		if (size == 0)
		{
			delete bmp;
			GdiplusShutdown(gdiplusToken);
			throw std::runtime_error("No image encoders found");
		}
		std::vector<BYTE> buffer(size);
		ImageCodecInfo*   pImageCodecInfo = reinterpret_cast<ImageCodecInfo*>(buffer.data());
		GetImageEncoders(numEncoders, size, pImageCodecInfo);

		bool found = false;
		for (auto j = 0; j < numEncoders; ++j)
		{
			if (wcscmp(pImageCodecInfo[j].MimeType, L"image/x-icon") == 0)
			{
				clsidEncoder = pImageCodecInfo[j].Clsid;
				found        = true;
				break;
			}
		}

		if (!found)
		{
			delete bmp;
			GdiplusShutdown(gdiplusToken);
			throw std::runtime_error("ICO encoder not found");
		}

		std::wstring woutput(output_path.begin(), output_path.end());
		if (bmp->Save(woutput.c_str(), &clsidEncoder, nullptr) != Ok)
		{
			delete bmp;
			GdiplusShutdown(gdiplusToken);
			throw std::runtime_error("Failed to save .ico file");
		}
		delete bmp;
		GdiplusShutdown(gdiplusToken);
		return output_path;
	}
	else
	{
		throw std::runtime_error("Not supported image file");
	}
}

void change_icon_cli(const std::int32_t argument_count,
                     const char** const arguments)
{
	assert(0 < argument_count);
	assert(nullptr != arguments);

	static constexpr std::uint16_t VERSION_MAJOR = 1;
	static constexpr std::uint16_t VERSION_MINOR = 1;
	static constexpr std::uint16_t VERSION_PATCH = 0;

	if (2 == argument_count && ("--version" == std::string_view{ arguments[1] } || "-v" == std::string_view{ arguments[1] }))
	{
		std::println("icon-changer version {}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
		return;
	}
	validate_argument_count(argument_count, arguments[0]);
	change_icon(arguments[1], arguments[2]);
	std::println(GRN "Icon changed successfully!" CRESET);
}

void change_icon_gui()
{
	throw std::runtime_error{ "GUI not yet implemented!" };
}

static void validate_argument_count(const std::int32_t     argument_count,
                                    const std::string_view program_path)
{
	static constexpr std::size_t REQUIRED_ARGUMENT_COUNT = 3;

	if (REQUIRED_ARGUMENT_COUNT == argument_count)
	{
		return;
	}

	std::println("Usage: {} <path_to_icon> <path_to_exe>", program_path);

	if (REQUIRED_ARGUMENT_COUNT > argument_count)
	{
		throw std::runtime_error{ std::format("{} parameter(s) missing!", REQUIRED_ARGUMENT_COUNT - argument_count) };
	}

	std::println(YEL "{} parameter(s) will be ignored..." CRESET, argument_count - REQUIRED_ARGUMENT_COUNT);
}

static void change_icon(const std::string_view icon_path,
                        const std::string_view executable_path)
{
	if (!std::filesystem::exists(icon_path))
	{
		throw std::invalid_argument{ std::format("\"{}\" does not exist!", icon_path) };
	}

	if (!std::filesystem::exists(executable_path))
	{
		throw std::invalid_argument{ std::format("\"{}\" does not exist!", executable_path) };
	}

	std::string prepared_icon;

	try
	{
		// checking file extension, and changing if needed
		prepared_icon = prepare_icon(icon_path);
	}
	catch (std::runtime_error& e)
	{
		std::println("Error: {}", e.what());
		return;
	}

	change_icon_s(prepared_icon, executable_path);
}

static void change_icon_s(const std::string_view icon_path,
                          const std::string_view executable_path)
{
	icon        icon         = { icon_path };
	void* const exe_resource = BeginUpdateResourceA(executable_path.data(), false);

	if (nullptr == exe_resource)
	{
		throw std::runtime_error{ "Failed to get executable's resource handle!" };
	}

	try
	{
		set_images(exe_resource, icon);
		set_icon_header(exe_resource, icon);
	}
	catch (const std::exception& exception)
	{
		EndUpdateResourceA(exe_resource, true);
		throw;
	}

	if (!EndUpdateResourceA(exe_resource, false))
	{
		throw std::runtime_error{ "Failed to commit the changes to the executable!" };
	}
}

static void set_images(void* const exe_resource,
                       icon&       icon)
{
	assert(nullptr != exe_resource);

	std::size_t id = 0;

	for (std::vector<std::uint8_t>& image : icon.get_images())
	{
		// We rely on the fact that we know IDs start from 1 in the header entries.
		if (!UpdateResourceA(exe_resource, RT_ICON, reinterpret_cast<char*>(++id), LANG_NEUTRAL, image.data(), image.size()))
		{
			throw std::runtime_error{ std::format("Failed to add RT_ICON resource with id {} to executable!", id) };
		}
	}
}

static void set_icon_header(void* const exe_resource,
                            const icon& icon)
{
	assert(nullptr != exe_resource);

	std::vector<std::uint8_t> header = icon.get_header();

	if (!UpdateResourceA(exe_resource, RT_GROUP_ICON, "MAINICON", LANG_NEUTRAL, header.data(), header.size()))
	{
		throw std::runtime_error{ "Failed to add RT_GROUP_ICON resource to executable!" };
	}
}

} // namespace icon_changer
