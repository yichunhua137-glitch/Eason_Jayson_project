module;

#include <iosfwd>
#include <string>

export module textdisplay;

import board;

export class TextDisplay {
  private:
    const Board &board;

  public:
    explicit TextDisplay(const Board &board);
    std::string render() const;
    std::string renderFormal() const;
    std::string renderDebug() const;
};

export std::ostream &operator<<(
    std::ostream &out,
    const TextDisplay &display
);
