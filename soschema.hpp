
#if !defined(SOSCHEMA_HPP_INCLUDED)
#define SOSCHEMA_HPP_INCLUDED

#include "dbspec.hpp"

// ---------------------------------------------------------------------------
// Table specifications

namespace {

const column_spec users_columns[] =
		{{"Id",             CT_INT},
		 {"Reputation",     CT_INT},
		 {"CreationDate",   CT_DATE},
		 {"DisplayName",    CT_VCHR64},
		 {"LastAccessDate", CT_DATE},
		 {"WebsiteUrl",     CT_TEXT},
		 {"Location",       CT_TEXT},
		 {"Age",            CT_INT},
		 {"AboutMe",        CT_TEXT},
		 {"Views",          CT_INT},
		 {"UpVotes",        CT_INT},
		 {"DownVotes",      CT_INT},
		 {0}};
const table_spec users_table = {"users", users_columns};

const column_spec badges_columns[] =
		{{"Id",             CT_INT},
		 {"UserId",         CT_INT},
		 {"Name",           CT_VCHR64},
		 {"Date",           CT_DATE},
		 {0}};
const table_spec badges_table = {"badges", badges_columns};

const column_spec votes_columns[] =
		{{"Id",             CT_INT},
		 {"PostId",         CT_INT},
		 {"VoteTypeId",     CT_INT},
		 {"UserId",         CT_INT},
		 {"CreationDate",   CT_DATE},
		 {"BountyAmount",   CT_INT},
		 {0}};
const table_spec votes_table = {"votes", votes_columns};

const column_spec posts_columns[] =
		{{"Id",             CT_INT},
		 {"PostTypeId",     CT_INT},
		 {"ParentId",       CT_INT},
		 {"AcceptedAnswerId", CT_INT},
		 {"CreationDate",   CT_DATE},
		 {"Score",          CT_INT},
		 {"ViewCount",      CT_INT},
		 {"Body",           CT_TEXT},
		 {"OwnerUserId",    CT_INT},
		 {"LastEditorUserId", CT_INT},
		 {"LastEditorDisplayName", CT_VCHR64},
		 {"LastEditDate",   CT_DATE},
		 {"LastActivityDate", CT_DATE},
		 {"CommunityOwnedDate", CT_DATE},
		 {"ClosedDate",     CT_DATE},
		 {"Title",          CT_TEXT},
		 {"Tags",           CT_TEXT},
		 {"AnswerCount",    CT_INT},
		 {"CommentCount",   CT_INT},
		 {"FavoriteCount",  CT_INT},
		 {0}};
const table_spec posts_table = {"posts", posts_columns};

const column_spec comments_columns[] =
		{{"Id",             CT_INT},
		 {"PostId",         CT_INT},
		 {"Score",          CT_INT},
		 {"Text",           CT_TEXT},
		 {"CreationDate",   CT_DATE},
		 {"UserId",         CT_INT},
		 {0}};
const table_spec comments_table = {"comments", comments_columns};

const size_t table_count = 5;
const table_spec tables[table_count] = {
	users_table,
	badges_table,
	votes_table,
	posts_table,
	comments_table
};

} // namepace

#endif // SOSCHEMA_HPP_INCLUDED

