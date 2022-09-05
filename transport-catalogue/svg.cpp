#include "svg.h"

namespace svg {
using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\""sv;
    RenderAttrs(context.out);
    out << "/>"sv;
}
    
// ---------- Polyline ------------------
    
Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(std::move(point));
    return *this;
}
    
void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    int i = 0;
    for (const auto& p : points_) {
        if (i!=0) { out << " "sv;}
        out << p.x <<","sv << p.y;
        i = 1;
    }
    out << "\" "sv;
    RenderAttrs(context.out);
    out << "/>"sv;
}
    
// ---------- Text ------------------
    
Text& Text::SetPosition(Point pos) {
    pos_ = pos;
    return *this;
}
    
Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}
    
Text& Text::SetFontSize(uint32_t size) {
    size_ = size;
    return *this;
}
    
Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}

Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}
    
Text& Text::SetData(std::string data) {
    data_ = data;
    return *this;
}
    
void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text x=\""sv << pos_.x <<"\" y=\""sv << pos_.y << "\""sv;
    RenderAttrs(context.out);
    out << " dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\""sv;
    out << " font-size=\""sv << size_ << "\""sv;
    if (font_weight_ != ""s) {
        out << " font-weight=\""sv << font_weight_ << "\""sv;
    }
    if (font_family_ != ""s) {
        out << " font-family=\""sv << font_family_ << "\""sv;
    }
    out << ">"sv;
    for (auto i : data_) {
        if (i == '"') {
            out << "&quot;"sv;
        }
        if (i == '\'') {
            out << "&apos;"sv;
        }
        if (i == '<') {
            out << "&lt;"sv;
        }  
        if (i == '>') {
            out << "&gt;"sv;
        }   
        if (i == '&') {
            out << "&amp;"sv;
        }
        else {
            out << i;
        }
    }
    out << "</text>"sv ;
}

// ---------- Document & ObjectContainer ------------------
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.push_back(std::move(obj));
}
    
void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv<< std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv<< std::endl;
    for (const auto& obj : objects_) {
        RenderContext ctx(out, 2, 2);
        (*obj).Render(ctx);
    }
    out << "</svg>"s<< std::endl;
}

std::ostream& operator<<(std::ostream& out, const StrokeLineCap line) {
    if (line == StrokeLineCap::BUTT) {
        out << "butt"sv;
    }
    if (line == StrokeLineCap::ROUND) {
        out << "round"sv;
    }
    if (line == StrokeLineCap::SQUARE) {
        out << "square"sv;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const StrokeLineJoin line) {
    if (line == StrokeLineJoin::ARCS) {
        out << "arcs"sv;
    }
    if (line == StrokeLineJoin::BEVEL) {
        out << "bevel"sv;
    }
    if (line == StrokeLineJoin::MITER) {
        out << "miter"sv;
    }
    if (line == StrokeLineJoin::MITER_CLIP) {
        out << "miter-clip"sv;
    }
    if (line == StrokeLineJoin::ROUND) {
        out << "round"sv;
    }
    return out;
}
}  // namespace svg