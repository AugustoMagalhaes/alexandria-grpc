#pragma once

#include "book.pb.h"
#include "common.pb.h"
#include "domain/Book.h"
#include "domain/User.h"
#include "user.pb.h"

namespace converters {

inline alexandria::v1::Role toProto(Role role)
{
    return role == Role::Admin ? alexandria::v1::ROLE_ADMIN : alexandria::v1::ROLE_USER;
}

inline Role fromProto(alexandria::v1::Role role)
{
    return role == alexandria::v1::ROLE_ADMIN ? Role::Admin : Role::User;
}

inline alexandria::v1::Book toProto(const Book& book)
{
    alexandria::v1::Book proto;
    proto.set_id(book.id);
    proto.set_title(book.title);
    proto.set_author(book.author);
    proto.set_isbn(book.isbn);
    proto.set_total_copies(book.totalCopies);
    proto.set_available_copies(book.availableCopies);
    return proto;
}

inline Book fromProto(const alexandria::v1::Book& proto)
{
    Book book;
    book.id = proto.id();
    book.title = proto.title();
    book.author = proto.author();
    book.isbn = proto.isbn();
    book.totalCopies = proto.total_copies();
    book.availableCopies = proto.available_copies();
    return book;
}

inline alexandria::v1::User toProto(const User& user)
{
    alexandria::v1::User proto;
    proto.set_id(user.id);
    proto.set_username(user.username);
    proto.set_role(toProto(user.role));
    return proto;
}

}