
#if !defined(SOSCHEMA_HPP_INCLUDED)
#define SOSCHEMA_HPP_INCLUDED

#include <stdlib.h>

// ---------------------------------------------------------------------------
// Table specifications

struct table_spec {
	const char *name;
	const char **columns;
};

namespace {

const char * users_columns[] =
		{"Id", "Reputation", "CreationDate", "DisplayName",
		 "LastAccessDate", "WebsiteUrl", "Location", "Age", "AboutMe",
		 "Views", "UpVotes", "DownVotes", 0};

const char * badges_columns[] =
		{"Id", "UserId", "Name", "Date", 0};

const char * votes_columns[] =
		{"Id", "PostId", "VoteTypeId", "UserId", "CreationDate",
		 "BountyAmount", 0};

const char * posts_columns[] =
		{"Id", "PostTypeId", "ParentId", "AcceptedAnswerId", "CreationDate",
		 "Score", "ViewCount", "Body", "OwnerUserId", "LastEditorUserId",
		 "LastEditorDisplayName", "LastEditDate", "LastActivityDate",
		 "CommunityOwnedDate", "ClosedDate", "Title", "Tags", "AnswerCount",
		 "CommentCount", "FavoriteCount", 0};

const char * comments_columns[] =
		{"Id", "PostId", "Score", "Text", "CreationDate", "UserId", 0};

const size_t table_count = 5;
const table_spec tables[table_count] = {
	{"users", users_columns},
	{"badges", badges_columns},
	{"votes", votes_columns},
	{"posts", posts_columns},
	{"comments", comments_columns}
};

} // namepace

#endif // SOSCHEMA_HPP_INCLUDED

