// format.hpp -- auconv

#ifndef AUCONV_FORMAT_HPP
#define AUCONV_FORMAT_HPP

#include <memory>

namespace auconv {

template <typename T>
class Format {
private:
    struct FormatConcept {
        virtual ~FormatConcept() = default;

        virtual void doConvert(const Format& fmt) const = 0;

        // `clone()` allows copying the FormatConcept object
        virtual std::unique_ptr<FormatConcept> clone() const = 0;
    };

    template <typename FormatT>
    struct FormatModel : public FormatConcept {
        FormatModel(FormatT fmt)
            : fmt_ {std::move(fmt)}
        { }

        std::unique_ptr<FormatConcept> clone() const override
        {
            return std::make_unique<FormatModel>(*this);
        }

        void doConvert(const Format& out) const override
        {
            convert(fmt_, out);
        }

        FormatT fmt_;
    };

    std::unique_ptr<FormatConcept> pimpl_;

    template <typename FormatT, typename ConvertStrat>
    struct ExtendedFormatModel : public FormatConcept {
        explicit ExtendedFormatModel(FormatT fmt, ConvertStrat converter)
            : fmt_ {std::move(fmt)}
            , converter_ {std::move(converter)}
        { }

        FormatT      fmt_;
        ConvertStrat converter_;
    };

    friend void convert(const Format& in, const Format& out)
    {
        in.pimpl_->doConvert(out);
    }

public:
    template <typename FormatT>
    explicit Format(FormatT fmt)
        : pimpl_ {std::make_unique<FormatModel<FormatT>>(std::move(fmt))}
    { }

    Format(const Format& other)
        : pimpl_ {other.pimpl_->clone()}
    { }

    Format& operator=(const Format& other)
    {
        other.pimpl_->clone().swap(pimpl_);
        return *this;
    }

    // Leaving the move operations undefined falls back on copy operations
    // Defining move operator= with swap, but not move ctor, can provide a noexcept move operation
    Format(Format&& other)            = default;
    Format& operator=(Format&& other) = default;
};

class Flac {
public:
    explicit Flac(int file)
        : file_ {file}
    { }

private:
    int file_ = 0;
};

/// Converts a file from Format `in` to Format `out`
template <typename InFormatT, typename OutFormatT>
void convert(const Format<InFormatT>& in, const Format<OutFormatT>& out)
{
    // ...
    /// TODO: (MATT) How to handle conversion from any Format to any Format?
}

} // namespace auconv

#endif // AUCONV_FORMAT_HPP