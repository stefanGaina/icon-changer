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
#include <print>
#include <stdexcept>
#include <vector>
#include <windows.h>

#include "icon.hpp"
#include "utility.hpp"

////////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

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
static void set_images(void*                                   exe_resource,
                       std::vector<std::vector<std::uint8_t>>& icon_images);

///
/// \brief Adds the group icon header (NEWHEADER + RESDIR) to the executable.
/// \param exe_resource: Handle to the open resource section of the executable.
/// \param icon: The parsed icon object containing the group icon header.
///
static void set_icon_header(void*                      exe_resource,
                            std::vector<std::uint8_t>& icon_header);

////////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

void change_icon(const std::string_view icon_path,
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

	change_icon_s(icon_path, executable_path);
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
		set_images(exe_resource, icon.get_images());
		set_icon_header(exe_resource, icon.get_header());
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

static void set_images(void* const                             exe_resource,
                       std::vector<std::vector<std::uint8_t>>& icon_images)
{
	assert(nullptr != exe_resource);

	std::size_t id = 0;

	for (std::vector<std::uint8_t>& image : icon_images)
	{
		// We rely on the fact that we know IDs start from 1 in the header entries.
		if (!UpdateResourceA(exe_resource, RT_ICON, reinterpret_cast<char*>(++id), LANG_NEUTRAL, image.data(), image.size()))
		{
			throw std::runtime_error{ std::format("Failed to add RT_ICON resource with id {} to executable!", id) };
		}
	}
}

static void set_icon_header(void* const                exe_resource,
                            std::vector<std::uint8_t>& icon_header)
{
	assert(nullptr != exe_resource);

	if (!UpdateResourceA(exe_resource, RT_GROUP_ICON, "MAINICON", LANG_NEUTRAL, icon_header.data(), icon_header.size()))
	{
		throw std::runtime_error{ "Failed to add RT_GROUP_ICON resource to executable!" };
	}
}

} // namespace icon_changer
