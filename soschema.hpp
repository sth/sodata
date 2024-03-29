
#if !defined(SOSCHEMA_HPP_INCLUDED)
#define SOSCHEMA_HPP_INCLUDED

#include "dbspec.hpp"

#include <vector>

// ---------------------------------------------------------------------------
// Table specifications

namespace {

const std::vector<column_spec> users_columns =
		{{"Id",             CT_INT},
		 {"Reputation",     CT_INT},
		 {"CreationDate",   CT_DATE},
		 {"DisplayName",    CT_VCHR64}, // 32 would also be enough
		 {"EmailHash",      CT_TEXT}, // obsolete, now always blank
		 {"LastAccessDate", CT_DATE},
		 {"WebsiteUrl",     CT_TEXT},
		 {"Location",       CT_TEXT},
		 {"AboutMe",        CT_TEXT},
		 {"Views",          CT_INT},
		 {"UpVotes",        CT_INT},
		 {"DownVotes",      CT_INT},
		 {"ProfileImageUrl", CT_TEXT},
		 {"Age",            CT_INT},
		 {"AccountId",      CT_INT},
		};
const table_spec users_table = {"Users", users_columns};

const std::vector<column_spec> badges_columns =
		{{"Id",             CT_INT},
		 {"UserId",         CT_INT},
		 {"Name",           CT_VCHR64},
		 {"Date",           CT_DATE},
		 {"Class",          CT_INT},
		 {"TagBased",       CT_BOOL},
		};
const table_spec badges_table = {"Badges", badges_columns};

const std::vector<column_spec> votes_columns =
		{{"Id",             CT_INT},
		 {"PostId",         CT_INT},
		 {"VoteTypeId",     CT_INT},
		 {"UserId",         CT_INT},
		 {"CreationDate",   CT_DATE},
		 {"BountyAmount",   CT_INT},
		};
const table_spec votes_table = {"Votes", votes_columns};

const std::vector<column_spec> posts_columns =
		{{"Id",             CT_INT},
		 {"PostTypeId",     CT_INT},
		 {"ParentId",       CT_INT},
		 {"AcceptedAnswerId", CT_INT},
		 {"CreationDate",   CT_DATE},
		 {"Score",          CT_INT},
		 {"ViewCount",      CT_INT},
		 {"Body",           CT_TEXT},
		 {"OwnerUserId",    CT_INT},
		 {"OwnerDisplayName", CT_VCHR64},
		 {"LastEditorUserId", CT_INT},
		 {"LastEditorDisplayName", CT_VCHR64},
		 {"LastEditDate",   CT_DATE},
		 {"LastActivityDate", CT_DATE},
		 {"Title",          CT_TEXT},
		 {"Tags",           CT_TEXT},
		 {"AnswerCount",    CT_INT},
		 {"CommentCount",   CT_INT},
		 {"FavoriteCount",  CT_INT},
		 {"ClosedDate",     CT_DATE},
		 {"CommunityOwnedDate", CT_DATE},
		 {"ContentLicense", CT_VCHR12},
		};
const table_spec posts_table = {"Posts", posts_columns};

const std::vector<column_spec> comments_columns =
		{{"Id",             CT_INT},
		 {"PostId",         CT_INT},
		 {"Score",          CT_INT},
		 {"Text",           CT_TEXT},
		 {"CreationDate",   CT_DATE},
		 {"UserDisplayName", CT_VCHR64},
		 {"UserId",         CT_INT},
		 {"ContentLicense", CT_VCHR12},
		};
const table_spec comments_table = {"Comments", comments_columns};

const std::vector<column_spec> posthistory_columns =
		{{"Id",             CT_INT},
		 {"PostHistoryTypeId", CT_INT},
		 {"PostId",         CT_INT},
		 {"RevisionGUID",   CT_VCHR64},
		 {"CreationDate",   CT_DATE},
		 {"UserId",         CT_INT},
		 {"UserDisplayName",CT_VCHR64},
		 {"Comment",        CT_TEXT},
		 {"CloseReasonId",  CT_INT}, // Does this still exist?
		 {"Text",           CT_TEXT},
		 {"ContentLicense", CT_VCHR12},
		};
const table_spec posthistory_table = {"PostHistory", posthistory_columns};

const std::vector<column_spec> postlinks_columns =
		{{"Id",             CT_INT},
		 {"CreationDate",   CT_DATE},
		 {"PostId",         CT_INT},
		 {"RelatedPostId",  CT_INT},
		 {"LinkTypeId",     CT_INT},
		};
const table_spec postlinks_table = {"PostLinks", postlinks_columns};

const std::vector<column_spec> tags_columns =
		{{"Id",             CT_INT},
		 {"TagName",        CT_VCHR64},
		 {"Count",          CT_INT},
		 {"ExcerptPostId",  CT_INT},
		 {"WikiPostId",     CT_INT},
		};
const table_spec tags_table = {"Tags", tags_columns};

const std::vector<table_spec> tables = {
	users_table,
	badges_table,
	votes_table,
	posts_table,
	comments_table,
	posthistory_table,
	postlinks_table,
	tags_table,
};

const std::vector<column_spec> posttags_columns =
		{{"PostId",         CT_INT},
		 {"Name",           CT_VCHR64},
		};
const table_spec posttags_table = {"PostTags", posttags_columns};

} // namespace

#endif // SOSCHEMA_HPP_INCLUDED

