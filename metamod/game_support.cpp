// vi: set ts=4 sw=4 :
// vim: set tw=75 :

// game_support.cpp - info to recognize different HL mod "games"

/*
 * Copyright (c) 2001-2003 Will Day <willday@hpgx.net>
 *
 *    This file is part of Metamod.
 *
 *    Metamod is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    Metamod is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Metamod; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of this program with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */

#include <extdll.h>			// always

#include "game_support.h"	// me
#include "log_meta.h"		// META_LOG, etc
#include "types_meta.h"		// mBOOL
#include "osdep.h"			// win32 snprintf, etc

// Adapted from adminmod h_export.cpp:
//! this structure contains a list of supported mods and their dlls names
//! To add support for another mod add an entry here, and add all the 
//! exported entities to link_func.cpp
game_modlist_t known_games = {
	// name/gamedir	 linux_so			win_dll			desc
	//
	// Previously enumerated in this sourcefile, the list is now kept in a
	// separate file, generated based on game information stored in a 
	// convenient db.
	//
#include "games.h"
	// End of list terminator:
	{NULL, NULL, NULL, NULL}
};

// Find a modinfo corresponding to the given game name.
game_modinfo_t *lookup_game(const char *name) {
	game_modinfo_t *imod;
	int i;
	for(i=0; known_games[i].name; i++) {
		imod=&known_games[i];
		if(strmatch(imod->name, name))
			return(imod);
	}
	// no match found
	return(NULL);
}

// Set all the fields in the gamedll struct, - based either on an entry in
// known_games matching the current gamedir, or on one specified manually 
// by the server admin.
//
// meta_errno values:
//  - ME_NOTFOUND	couldn't recognize game
mBOOL setup_gamedll(gamedll_t *gamedll) {
	static char override_desc_buf[256];
	game_modinfo_t *known;
	char *cp, *knownfn;
	int override=0;

	// Check for old-style "metagame.ini" file and complain.
	if(valid_gamedir_file(OLD_GAMEDLL_TXT))
		META_ERROR("File '%s' is no longer supported; instead, specify override gamedll in %s or with '+localinfo mm_gamedll <dllfile>'", OLD_GAMEDLL_TXT, CONFIG_INI);

	// First, look for a known game, based on gamedir.
	if((known=lookup_game(gamedll->name))) {
#ifdef _WIN32
		knownfn=known->win_dll;
#elif defined(linux)
		knownfn=known->linux_so;
#else
#error "OS unrecognized"
#endif /* _WIN32 */
	}

	// Neither override nor auto-detect found a gamedll.
	if(!known && !Config->gamedll)
			RETURN_ERRNO(mFALSE, ME_NOTFOUND);

	// Use override-dll if specified.
	if(Config->gamedll) {
		STRNCPY(gamedll->pathname, Config->gamedll, 
				sizeof(gamedll->pathname));
		override=1;
	}
	// Else use Auto-detect dll.
	else {
		snprintf(gamedll->pathname, sizeof(gamedll->pathname), "%s/dlls/%s", 
				gamedll->gamedir, knownfn);
	}

	// get filename from pathname
	cp=strrchr(gamedll->pathname, '/');
	if(cp) cp++;
	else cp=gamedll->pathname;
	gamedll->file=cp;

	// If found, store also the supposed "real" dll path based on the
	// gamedir, in case it differs from the "override" dll path.
	if(known && override)
		snprintf(gamedll->real_pathname, sizeof(gamedll->real_pathname),
				"%s/dlls/%s", gamedll->gamedir, knownfn);
	else // !known or !override
		STRNCPY(gamedll->real_pathname, gamedll->pathname, 
				sizeof(gamedll->real_pathname));
	
	if(override) {
		// generate a desc
		snprintf(override_desc_buf, sizeof(override_desc_buf), 
				"%s (override)", gamedll->file);
		gamedll->desc=override_desc_buf;
		// log result
		META_LOG("Overriding game '%s' with dllfile '%s'", gamedll->name, 
				gamedll->file);
	}
	else {
		gamedll->desc=known->desc;
		META_LOG("Recognized game '%s'; using dllfile '%s'", gamedll->name, 
				gamedll->file);
	}
	return(mTRUE);
}
