/*

	Creator : LYZENCORE 9 (DISTROXYDE 8) | DX8 - LYZC9

	Version : 0.1 (Alpha)

	Based On VYD Cores : NO

	Special Thanks For : DracoBlue And JaTochNietDan

	Created Data : 16/12/2023 15:18

	Release Date : 16/12/2023 16:20


*/

#if defined _UDINI3_INC
  #endinput
#endif

#define _UINI3_INC


#if defined MAX_STRING_ON_INC_UDINI
	#define UINI_MAX_STRING MAX_STRING_ON_INC_UDINI
#else
	#define UINI_MAX_STRING 256
#endif

#include <a_samp>
#include <filemanager>



stock UINI_Exists(file[]) 
{
	return file_exists(file);
}

stock UINI_Move(file[], loc[])
{
	return file_move(file, loc);
}

stock UINI_Copy(file[], loc[])
{
	return file_copy(file, loc);
}

stock UINI_Create(file[])
{
	return file_create(file);
}

stock UINI_Delete(file[])			// Some People Want That
{
	return file_delete(file);
}

// Set Data Part

stock UINI_SetInt(file[], key[], value)
{
	new val[UINI_MAX_STRING];
	format(val, sizeof(val), "%d", value);
	return UINI_SetString(file, key, val);
}

stock UINI_SetFloat(file[], key[], Float:value)
{
	new val[UINI_MAX_STRING];
	format(val, sizeof(val), "%f", value);
	return UINI_SetString(file, key, val);
}

stock UINI_SetBool(file[], key[], value)
{
	if(value)
	{
		return UINI_Set(file, key, "1");
	}
	return UINI_SetString(file, key, "0");
}


stock UINI_SetString(filename[], key[], value[]) {
	// If we have no key, it can't be set
	// we also have no chance to set the value, if all together is bigger then the max string
	new key_length = strlen(key);
	new value_length = strlen(value);
	if (key_length==0 || key_length+value_length+2>UINI_MAX_STRING) return false;
	
	new File:fohnd, File:fwhnd;
	new tmpres[UINI_MAX_STRING];
	new bool:wasset=false;
	
	format(tmpres, sizeof(tmpres), "%s.part", filename);
	file_delete(tmpres);
	fohnd=f_open(filename, "r");
	if (!fohnd) return false;
	
	fwhnd=f_open(tmpres, "w");
	if (!fwhnd) {
		f_close(fohnd);
		return false;
	}
	
	while (f_read(fohnd, tmpres)) {
		if (
			!wasset
			&& tmpres[key_length]=='='
			&& !strcmp(tmpres, key, true, key_length)	
		) {
				format(tmpres,sizeof(tmpres),"%s=%s",key,value);
				wasset=true;
		} else {
			StripNewLine(tmpres);
		}
		f_write(fwhnd,tmpres);
		f_write(fwhnd,"\r\n");
	}

	if (!wasset) {
		format(tmpres,sizeof(tmpres),"%s=%s",key,value);
		f_write(fwhnd,tmpres);
		f_write(fwhnd,"\r\n");
	}

	f_close(fohnd);
	f_close(fwhnd);

	format(tmpres,sizeof(tmpres),"%s.part",filename);
	if (copyfile(tmpres,filename)) {
		return file_delete(tmpres);
	}
	return false;
}

// Get Data Part

stock UINI_GetString(file[], key[]) 
{
	new tmpres[UINI_MAX_STRING];
	
	new key_length = strlen(key);
	if (key_length==0 || key_length+2>UINI_MAX_STRING) return tmpres;
	
	new File:fohnd;
	fohnd=f_open(filename,"r");
	if (!fohnd) return tmpres;
	
	while (f_read(fohnd,tmpres)) {
		if (
			tmpres[key_length]=='='
			&& !strcmp(tmpres, key, true, key_length)	
		) {
			/* We've got what we need */
			StripNewLine(tmpres);
			strmid(tmpres, tmpres, key_length + 1, strlen(tmpres), UINI_MAX_STRING);
			f_close(fohnd);
			return tmpres;
		}
	}
	f_close(fohnd);
	return tmpres;
}

stock UINI_GetInt(file[], key[])
{
	return strval(UINI_GetString(file, key));
}

stock Float:UINI_GetFloat(file[], key[])
{
	return floatstr(UINI_GetString(file, key));
}

stock UINI_GetBool(file[], key[])
{
	return strval(UINI_GetString(file, key));
}

stock StripNewLine(string[]) {
	new len = strlen(string);
	if (string[0]==0) return ;
	if ((string[len - 1] == '\n') || (string[len - 1] == '\r')) 
	{
		string[len - 1] = 0;
		if (string[0]==0) return ;
		if ((string[len - 2] == '\n') || (string[len - 2] == '\r')) string[len - 2] = 0;
	}
}

stock UINI_IsSet(file[], key[]) {
	new key_length = strlen(key);
	if (key_length==0 || key_length+2>UINI_MAX_STRING) return false;
	
	new File:fohnd;
	fohnd=f_open(file, "r");
	if (!fohnd) return false;
	
	new tmpres[UINI_MAX_STRING];
	while (f_read(fohnd,tmpres)) {
		if (
				tmpres[key_length]=='='
			&&  !strcmp(tmpres, key, true, key_length)	
		) {
			// We've got what we need
			f_close(fohnd);
			return true;
		}
	}
	f_close(fohnd);
	return false;
}


stock UINI_UnSet(file[], key[]) {
	// If we have no key, it can't be set
	// we also have no chance to unset the key, if all together is bigger then the max string
	new key_length = strlen(key);
	if (key_length==0 || key_length+2>UINI_MAX_STRING) return false;
	
	new File:fohnd, File:fwhnd;
	new tmpres[UINI_MAX_STRING];
	
	// Let's remove the old *.part file if there was one.
	format(tmpres,UINI_MAX_STRING,"%s.part", file);
	file_delete(tmpres);
	
	// We'll open the source file.
	fohnd=f_open(file, "r");
	if (!fohnd) return false;
	
	fwhnd=f_open(tmpres, "w");
	if (!fwhnd) {
		// we can't open the second file for writing, so .. let's close the open one and exit.
		f_close(fohnd);
		return false;
	}
	
	while (f_read(fohnd,tmpres)) {
		if (
			tmpres[key_length]=='='
			&& !strcmp(tmpres, key, true, key_length)	
		) {
				// We've got what needs to be removed!
		} else {
			StripNewLine(tmpres);
			f_write(fwhnd,tmpres);
			f_write(fwhnd,"\r\n");
		}
	}
	
	f_close(fohnd);
	f_close(fwhnd);

	format(tmpres,UINI_MAX_STRING,"%s.part", file);
	if (copyfile(tmpres, file)) {
		return file_delete(tmpres);
	}
	return false;
}

stock copyfile(oldname[], newname[]) {
	new File:ohnd,File:nhnd;
	if (!file_exists(oldname)) return false;
	ohnd=f_open(oldname, "r");
	if (!ohnd) return false;
	nhnd=f_open(newname, "w");
	if (!nhnd) {
		f_close(ohnd);
		return false;
	}
	new tmpres[UINI_MAX_STRING];
	while (f_read(ohnd,tmpres)) {
		StripNewLine(tmpres);
		format(tmpres,sizeof(tmpres),"%s\r\n",tmpres);
		f_write(nhnd,tmpres);
	}
	f_close(ohnd);
	f_close(nhnd);
	return true;
}


stock UINI_CreateFolder(path[])
{
	return dir_create(path);
}

stock UINI_OpenFolder(path[])
{
	return dir_open(path);
}

stock UINI_DeleteFolder(path[])
{
	return dir_delete(path);
}

stock UINI_FolderExists(path[])
{
	return dir_exists(path);
}

stock UINI_CloseFolder(dir:handle)
{
	return dir_close(dir:handle);
}

stock UINI_List(dir:handle, storage[], &type, length = sizeof(storage))
{
	dir_list(dir:handle, storage[], type, length = sizeof(storage);
}

#define FM_DIR 1
#define FM_FILE 2
