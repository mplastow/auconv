// format.hpp -- auconv

// Format: A generic class for representing and doing operations on any audio format
// Implemented using type erasure (the templated concept-model kind)

#ifndef AUCONV_FORMAT_HPP
#define AUCONV_FORMAT_HPP

#include <array>
#include <iostream>
#include <memory>

namespace auconv {

/// Format: uses type erasure to define generic conversion between any two audio formats
// template <typename T>
class Format {
private:
    // Format Concept: an empty base class that provides an interface to hide
    //  the implementation of any Format Model
    struct FormatConcept {
        virtual ~FormatConcept() = default;
        // `clone()` allows copying the FormatConcept object
        [[nodiscard]] virtual std::unique_ptr<FormatConcept> clone() const = 0;

        // Interface
        virtual void doConvert(const Format& out) const = 0;
    };

    // Pointer to the implementation of the Format Concept interface
    std::unique_ptr<FormatConcept> pimpl_;

    // Format Model: implements Format Concept (in this case, via calls to free
    //  functions)
    template <typename FormatT>
    struct FormatModel : public FormatConcept {
        FormatModel(FormatT fmt)
            : fmt_ {std::move(fmt)}
        { }

        [[nodiscard]] std::unique_ptr<FormatConcept> clone() const override
        {
            return std::make_unique<FormatModel>(*this);
        }

        // Implementation of Format Concept's interface via a call to a free
        //  function in the surrounding namespace
        void doConvert(const Format& out) const override
        {
            convert(fmt_, out);
        }

        FormatT fmt_;
    };

    // Extended Format Model: allows us to inject a conversion Strategy
    template <typename FormatT, typename ConvertStrat>
    struct ExtendedFormatModel : public FormatConcept {
        explicit ExtendedFormatModel(FormatT fmt, ConvertStrat converter)
            : fmt_ {std::move(fmt)}
            , converter_ {std::move(converter)}
        { }

        [[nodiscard]] std::unique_ptr<FormatConcept> clone() const override
        {
            return std::make_unique<ExtendedFormatModel>(*this);
        }

        // Implementation of Format Concept's interface
        void doConvert(const Format& out) const override
        {
            converter_(fmt_);
        }

        FormatT      fmt_;
        ConvertStrat converter_;
    };

    // A hidden friend function that triggers the polymorphic behavior for each
    //  concrete Format type
    friend void convert(const Format& in, const Format& out)
    {
        in.pimpl_->doConvert(out);
    }

public:
    // Constructor for a concrete Format type
    template <typename FormatT>
    explicit Format(FormatT fmt)
        : pimpl_ {std::make_unique<FormatModel<FormatT>>(std::move(fmt))}
    { }

    // Constructor for a concrete Extended Format type
    template <typename FormatT, typename ConvertStrat>
    Format(FormatT fmt, ConvertStrat converter)
        : pimpl_ {
              std::make_unique<ExtendedFormatModel<FormatT, ConvertStrat>>(
                  std::move(fmt), std::move(converter))}
    { }

    Format(const Format& other)
        : pimpl_ {other.pimpl_->clone()}
    { }

    Format& operator=(const Format& other)
    {
        other.pimpl_->clone().swap(pimpl_);
        return *this;
    }

    // Choices for the move operations
    // 1. Leaving the move operations undefined falls back on copy operations
    // 2. Defining move operator= with swap, but not move ctor, provides a
    //  noexcept move operation
    // 3. Defaulting move operations (as below) is efficient but has no noexcept
    Format(Format&& other)            = default;
    Format& operator=(Format&& other) = default;
};

constexpr size_t BUFFER_SIZE_DEFAULT {128};
constexpr size_t BUFFER_ALIGNMENT_DEFAULT {16};

/// FormatSBO: Format with a Small Buffer Optimization
template <size_t buffer_size = BUFFER_SIZE_DEFAULT, size_t alignment = BUFFER_ALIGNMENT_DEFAULT>
class FormatSBO {
private:
    // Format Concept: an empty base class that provides an interface to hide
    //  the implementation of any Format Model
    struct FormatConcept {
        virtual ~FormatConcept() = default;
        // `clone()` allows copying the FormatConcept object
        virtual void clone(FormatConcept* memory) const = 0;
        virtual void move(FormatConcept* memory) const  = 0;

        // Interface
        virtual void doConvert(const FormatSBO& fmt) const = 0;
    };

    // Format Model: implements Format Concept (in this case, via calls to free
    //  functions)
    template <typename FormatT>
    struct FormatModel : public FormatConcept {
        FormatModel(FormatT fmt)
            : fmt_ {std::move(fmt)}
        { }

        void clone(FormatConcept* memory) const override
        {
            ::new (memory) FormatModel {*this};
        }

        void move(FormatConcept* memory) const override
        {
            ::new (memory) FormatModel {std::move(*this)};
        }

        // Implementation of Format Concept's interface via a call to a free
        //  function in the surrounding namespace
        void doConvert(const FormatSBO& out) const override
        {
            convert(fmt_, out);
        }

        FormatT fmt_;
    };

    // A hidden friend function that triggers the polymorphic behavior for each
    //  concrete Format type
    friend void convert(const FormatSBO& in, const FormatSBO& out)
    {
        in.pimpl()->doConvert(out);
    }

    FormatConcept* pimpl() noexcept
    {
        return reinterpret_cast<FormatConcept*>(buffer_.data());
    }

    const FormatConcept* pimpl() const noexcept
    {
        return reinterpret_cast<const FormatConcept*>(buffer_.data());
    }

    // Small buffer optimization: store the implementation inside the object
    //  instead of using a pimpl
    // A 128-byte buffer with a max alignment of 16 bytes
    alignas(alignment) std::array<std::byte, buffer_size> buffer_;

public:
    // Constructor for a concrete Format type
    template <typename FormatT>
    explicit FormatSBO(const FormatT& fmt)
    {
        using M = FormatModel<FormatT>;
        static_assert(sizeof(M) <= buffer_size, "Given type is too large");
        static_assert(alignof(M) <= alignment, "Given type is overaligned");
        ::new (pimpl()) M {fmt};
    }

    // Destructor explicitly destroys the buffer
    ~FormatSBO() { pimpl()->~FormatConcept(); }

    // Copy operations
    FormatSBO(const FormatSBO& other)
    {
        other.pimpl()->clone(pimpl());
    }
    FormatSBO& operator=(const FormatSBO& other)
    {
        // Copy-and-swap
        FormatSBO copy {other};
        buffer_.swap(copy.buffer_);
        return *this;
    }

    // Move operations
    FormatSBO(FormatSBO&& other)
    {
        other.pimpl()->move(pimpl());
    }
    FormatSBO& operator=(FormatSBO&& other)
    {
        // Move-and-swap idiom
        FormatSBO tmp {std::move(other)};
        buffer_.swap(tmp.buffer_);
        return *this;
    }
};

// A non-owning view of a Format (also known as FormatConstRef)
class FormatView {
public:
    template <typename FormatT>
    explicit FormatView(const FormatT& fmt)
        : fmt_ {std::addressof(fmt)} // to avoid problems when `&` is overloaded
        , convert_ {
              // Initialize convert_ function pointer with a lambda that captures
              //    the passed-in Format and uses it as an argument to a free function
              /// NOTE: (MATT) May need to be modified to reflect the fact that our
              //    free function takes both an in Format and an out Format
              [](void const* fmt) {
                  convert(*static_cast<FormatT const*>(fmt));
              }}
    { }

private:
    friend void convert(const FormatView& fmt)
    {
        fmt.convert_(fmt.fmt_);
    }

    using ConvertOp = void(void const*);

    void const* fmt_ {nullptr};
    // Pointer to conversion operation
    ConvertOp* convert_ {nullptr};
};

class Flac {
public:
    explicit Flac(int file)
        : file_ {file}
    { }

private:
    int file_ = 1;
};

class Wav {
public:
    explicit Wav(int file)
        : file_ {file}
    { }

private:
    int file_ = 0;
};

/// Converts a file from Format `in` to Format `out`
template <typename InFormatT, typename OutFormatT>
void convert(const InFormatT& in, const OutFormatT& out)
{
    // ...
    /// TODO: (MATT) How to handle conversion from any Format to any Format?
    std::cout << " * FORMAT: We're converting!\n";
}

} // namespace auconv

#endif // AUCONV_FORMAT_HPP