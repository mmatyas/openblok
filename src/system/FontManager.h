#pragma once

#include <memory>
#include <string>
#include <vector>


class FontManager {
public:
    virtual ~FontManager() {}

    /// Return the path of the best matching font for a family name (eg. Arial, sans-serif),
    /// and for the optional style parameters (eg. italic, heavy oblique).
    virtual std::string find(const std::string& family, const std::string& style = "") = 0;
    /// Return the paths of all the fonts usable for a family name (eg. Arial, sans-serif),
    /// and for the optional style parameters (eg. italic, heavy oblique).
    virtual std::vector<std::string> findAll(const std::string& family, const std::string& style = "") = 0;

private:
    static std::unique_ptr<FontManager> create();

friend class AppContext;
};
