#include <cstring>

#include <sstream>
#include <algorithm>

#include <erl_nif.h>

#include <taglib/fileref.h>
#include <taglib/audioproperties.h>
#include <taglib/tag.h>
#include <taglib/mp4file.h>
#include <taglib/mp4item.h>
#include <taglib/mp4tag.h>
#include <taglib/mp4coverart.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/flacfile.h>
#include <taglib/xiphcomment.h>
#include <taglib/asffile.h>

#ifndef nullptr
    #define nullptr NULL
#endif

static ErlNifResourceType* taglib_nif_resource = nullptr;

typedef struct
{
    TagLib::File *taglib_file;
} taglib_nif_handle;

/* Turn a string into an integer type for any map class. */
template <typename T, typename M>
inline T extractTag(const M &map, const char *key)
{
	T ret = 0;
	std::istringstream stream(extractTag<std::string>(map, key));
	stream >> ret;
	return ret;
}
/* Turn a string into a pair of integer types separated by a slash
 * for any map class. */
template <typename T1, typename T2, typename M>
inline std::pair<T1, T2> extractTag(const M &map, const char *key)
{
	std::pair<T1, T2> values;
	values.first = 0;
	values.second = 0;
	char slash = '\0';
	std::istringstream stream(extractTag<std::string>(map, key));
	stream >> values.first >> slash >> values.second;
	if (slash != '/')
		values.second = 0;
	return values;
}
/* Extract a string out of an MP3 map. */
template <>
inline std::string extractTag<std::string, TagLib::ID3v2::FrameListMap>(const TagLib::ID3v2::FrameListMap &map, const char *key)
{
	if (map[key].isEmpty())
		return std::string();
	return map[key].front()->toString().to8Bit(true);
}
/* Turn a string into a bool, based on "1" and "true", for MP3. */
template <>
inline bool extractTag<bool, TagLib::ID3v2::FrameListMap>(const TagLib::ID3v2::FrameListMap &map, const char *key)
{
	std::string str(extractTag<std::string>(map, key));
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return (str == "1" || str == "true");
}
/* Extract a string out of an OGG map. */
template <>
inline std::string extractTag<std::string, TagLib::Ogg::FieldListMap>(const TagLib::Ogg::FieldListMap &map, const char *key)
{
	if (map[key].isEmpty())
		return std::string();
	return map[key].front().to8Bit(true);
}
/* Turn a string into a bool, based on "1" and "true", for OGG. */
template <>
inline bool extractTag<bool, TagLib::Ogg::FieldListMap>(const TagLib::Ogg::FieldListMap &map, const char *key)
{
	std::string str(extractTag<std::string>(map, key));
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return (str == "1" || str == "true");
}
/* Extract an integer pair out of an MP4 map. */
template <typename T1, typename T2>
inline std::pair<T1, T2> extractTag(const TagLib::MP4::ItemListMap &map, const char *key)
{
	if (!map[key].isValid())
		return std::pair<T1, T2>(0, 0);
	TagLib::MP4::Item::IntPair pair = map[key].toIntPair();
	return std::pair<T1, T2>(pair.first, pair.second);
}
/* Extract an integer out of an MP4 map. */
template <>
inline unsigned int extractTag<unsigned int, TagLib::MP4::ItemListMap>(const TagLib::MP4::ItemListMap &map, const char *key)
{
	if (!map[key].isValid())
		return 0;
	return map[key].toInt();
}
/* Extract a string out of an MP4 map. */
template <>
inline std::string extractTag<std::string, TagLib::MP4::ItemListMap>(const TagLib::MP4::ItemListMap &map, const char *key)
{
	if (!map[key].isValid())
		return std::string();
	return map[key].toStringList().toString().to8Bit(true);
}
/* Extract a bool out of an MP4 map. */
template <>
inline bool extractTag<bool, TagLib::MP4::ItemListMap>(const TagLib::MP4::ItemListMap &map, const char *key)
{
	if (!map[key].isValid())
		return false;
	return map[key].toBool();
}

static ERL_NIF_TERM enif_make_bool(ErlNifEnv* env, bool boolean){
    if(boolean) {
        return enif_make_atom(env, "true");
    } else {
        return enif_make_atom(env, "false");
    }
}

static std::string binary_to_string(ErlNifEnv* env, const ERL_NIF_TERM term)
{
    ErlNifBinary bin;
    if(!enif_inspect_binary(env, term, &bin)) {
        return std::string();
    }
    return std::string((const char*)bin.data, bin.size);
}

static ERL_NIF_TERM string_to_binary(ErlNifEnv* env, const TagLib::ByteVector& bytes)
{
    ErlNifBinary bin;
    enif_alloc_binary(bytes.size(), &bin);
    std::memcpy(bin.data, bytes.data(), bytes.size());
    return enif_make_binary(env, &bin);
}

static ERL_NIF_TERM string_to_binary(ErlNifEnv* env, const TagLib::String& str)
{
    return string_to_binary(env, str.data(TagLib::String::UTF8));
}

static ERL_NIF_TERM taglib_nif_resource_create(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    TagLib::File *file = TagLib::FileRef::create(binary_to_string(env, argv[0]).data());
    if(file->isValid()) {
        taglib_nif_handle *handle = static_cast<taglib_nif_handle*>(enif_alloc_resource(taglib_nif_resource, sizeof(taglib_nif_handle)));
        handle->taglib_file = file;

        ERL_NIF_TERM result = enif_make_resource(env, handle);
        enif_release_resource(handle);

        return enif_make_tuple(env, 2, enif_make_atom(env, "ok"), result);
    } else {
        return enif_make_tuple(env, 2, enif_make_atom(env, "error"), enif_make_atom(env, "invalid_file"));
    }
}

static void taglib_nif_resource_cleanup(ErlNifEnv* env, void* arg)
{
    taglib_nif_handle* handle = static_cast<taglib_nif_handle*>(arg);
    delete handle->taglib_file;
}

static TagLib::File *taglib_nif_resource_file(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    taglib_nif_handle* handle;
    if(enif_get_resource(env, argv[0], taglib_nif_resource, (void **)&handle)) {
        return handle->taglib_file;
    } else {
        return nullptr;
    }
}

static TagLib::Tag *taglib_nif_resource_tag(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if(TagLib::File *file = taglib_nif_resource_file(env, argc, argv)) {
        return file->tag();
    } else {
        return nullptr;
    }
}

static TagLib::AudioProperties *taglib_nif_resource_audio_props(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if(TagLib::File *file = taglib_nif_resource_file(env, argc, argv)) {
        return file->audioProperties();
    } else {
        return nullptr;
    }
}
static ERL_NIF_TERM tag_title(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if(TagLib::Tag *tag = taglib_nif_resource_tag(env, argc, argv)) {
        return string_to_binary(env, tag->title());
    } else {
        return enif_make_badarg(env);
    }
}

static ERL_NIF_TERM tag_artist(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if(TagLib::Tag *tag = taglib_nif_resource_tag(env, argc, argv)) {
        return string_to_binary(env, tag->artist());
    } else {
        return enif_make_badarg(env);
    }
}

static ERL_NIF_TERM tag_album(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if(TagLib::Tag *tag = taglib_nif_resource_tag(env, argc, argv)) {
        return string_to_binary(env, tag->album());
    } else {
        return enif_make_badarg(env);
    }
}

static ERL_NIF_TERM tag_genre(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if(TagLib::Tag *tag = taglib_nif_resource_tag(env, argc, argv)) {
        return string_to_binary(env, tag->genre());
    } else {
        return enif_make_badarg(env);
    }
}

static ERL_NIF_TERM tag_disc(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if(TagLib::File *file = taglib_nif_resource_file(env, argc, argv)) {
        if(TagLib::MP4::File *mp4 = dynamic_cast<TagLib::MP4::File *>(file)) {
            TagLib::MP4::Tag *tag = mp4->tag();
            const TagLib::MP4::ItemListMap &list_map = tag->itemListMap();
            std::pair<unsigned int, unsigned int> discPair = extractTag<unsigned int, unsigned int>(list_map, "disk");
            return enif_make_uint(env, discPair.first);
        } else if(TagLib::MPEG::File *mp3 = dynamic_cast<TagLib::MPEG::File *>(file)) {
            TagLib::ID3v2::Tag *tag = mp3->ID3v2Tag();
            const TagLib::ID3v2::FrameListMap &list_map = tag->frameListMap();
            std::pair<unsigned int, unsigned int> discPair = extractTag<unsigned int, unsigned int>(list_map, "TPOS");
            return enif_make_uint(env, discPair.first);
        } else if(TagLib::FLAC::File *flac = dynamic_cast<TagLib::FLAC::File *>(file)) {
            TagLib::Ogg::XiphComment *comment = flac->xiphComment();
            const TagLib::Ogg::FieldListMap &list_map = comment->fieldListMap();
            return enif_make_uint(env, extractTag<unsigned int>(list_map, "DISCNUMBER"));
        }
    } else {
        return enif_make_badarg(env);
    }
}

static ERL_NIF_TERM tag_track(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if(TagLib::Tag *tag = taglib_nif_resource_tag(env, argc, argv)) {
        return enif_make_uint(env, tag->track());
    } else {
        return enif_make_badarg(env);
    }
}

static ERL_NIF_TERM tag_year(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if(TagLib::Tag *tag = taglib_nif_resource_tag(env, argc, argv)) {
        return enif_make_uint(env, tag->year());
    } else {
        return enif_make_badarg(env);
    }
}

static ERL_NIF_TERM tag_compilation(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if(TagLib::File *file = taglib_nif_resource_file(env, argc, argv)) {
        if(TagLib::MP4::File *mp4 = dynamic_cast<TagLib::MP4::File *>(file)) {
            TagLib::MP4::Tag *tag = mp4->tag();
            const TagLib::MP4::ItemListMap &list_map = tag->itemListMap();
            return enif_make_bool(env, extractTag<bool>(list_map, "cpil"));
        } else if(TagLib::MPEG::File *mp3 = dynamic_cast<TagLib::MPEG::File *>(file)) {
            TagLib::ID3v2::Tag *tag = mp3->ID3v2Tag();
            const TagLib::ID3v2::FrameListMap &list_map = tag->frameListMap();
            return enif_make_bool(env, extractTag<bool>(list_map, "TCMP"));
        } else if(TagLib::FLAC::File *flac = dynamic_cast<TagLib::FLAC::File *>(file)) {
            TagLib::Ogg::XiphComment *comment = flac->xiphComment();
            const TagLib::Ogg::FieldListMap &list_map = comment->fieldListMap();
            return enif_make_bool(env, extractTag<bool>(list_map, "COMPILATION"));
        }
    } else {
        return enif_make_badarg(env);
    }
}
static ERL_NIF_TERM audio_length(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if(TagLib::AudioProperties *props = taglib_nif_resource_audio_props(env, argc, argv)) {
        return enif_make_int(env, props->length());
    } else {
        return enif_make_badarg(env);
    }
}

static ERL_NIF_TERM artwork_picture(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if(TagLib::File *file = taglib_nif_resource_file(env, argc, argv)) {
        if(TagLib::MP4::File *mp4 = dynamic_cast<TagLib::MP4::File *>(file)) {
            TagLib::MP4::Tag *tag = mp4->tag();
            TagLib::MP4::ItemListMap items = tag->itemListMap();
            TagLib::MP4::CoverArtList covers = items["covr"].toCoverArtList();
            if(!covers.isEmpty()) {
                TagLib::MP4::CoverArt cover = covers.front();
                TagLib::String mimetype;
                switch(cover.format()) {
                    case TagLib::MP4::CoverArt::Format::JPEG:
                        mimetype = "image/jpeg";
                        break;
                    case TagLib::MP4::CoverArt::Format::PNG:
                        mimetype = "image/png";
                        break;
                    case TagLib::MP4::CoverArt::Format::BMP:
                        mimetype = "image/bmp";
                        break;
                    case TagLib::MP4::CoverArt::Format::GIF:
                        mimetype = "image/gif";
                }
                return enif_make_tuple(env, 2, string_to_binary(env, mimetype), string_to_binary(env, cover.data()));
            }
        } else if(TagLib::MPEG::File *mp3 = dynamic_cast<TagLib::MPEG::File *>(file)) {
            TagLib::ID3v2::Tag *tag = mp3->ID3v2Tag();
            TagLib::ID3v2::FrameList covers = tag->frameList("APIC");
            if(!covers.isEmpty()) {
                TagLib::ID3v2::AttachedPictureFrame *frame = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(covers.front());
                TagLib::String mimetype = frame->mimeType();
                return enif_make_tuple(env, 2, string_to_binary(env, mimetype), string_to_binary(env, frame->picture()));
            }
        } else if(TagLib::FLAC::File *flac = dynamic_cast<TagLib::FLAC::File *>(file)) {
            TagLib::List<TagLib::FLAC::Picture *> covers = flac->pictureList();
            if(!covers.isEmpty()) {
                TagLib::FLAC::Picture *picture = covers.front();
                TagLib::String mimetype = picture->mimeType();
                return enif_make_tuple(env, 2, string_to_binary(env, mimetype), string_to_binary(env, picture->data()));
            }
        } else if(TagLib::ASF::File *asf = dynamic_cast<TagLib::ASF::File *>(file)) {
            TagLib::ASF::Tag *tag = static_cast<TagLib::ASF::Tag *>(asf->tag());
            TagLib::ASF::AttributeListMap attrs = tag->attributeListMap();
            TagLib::ASF::AttributeList covers = attrs["WM/Picture"];
            if(!covers.isEmpty()) {
                TagLib::ASF::Picture picture = covers.front().toPicture();
                TagLib::String mimetype = picture.mimeType();
                return enif_make_tuple(env, 2, string_to_binary(env, mimetype), string_to_binary(env, picture.picture()));
            }
        }
    }
}

static int taglib_nif_init(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info)
{
    ErlNifResourceFlags flags = static_cast<ErlNifResourceFlags>(ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER);
    ErlNifResourceType* rt = enif_open_resource_type(env, NULL, "taglib_nif_resource", &taglib_nif_resource_cleanup, flags, NULL);
    if(rt != nullptr) {
        taglib_nif_resource = rt;
        return 0;
    } else {
        return -1;
    }
}

static ErlNifFunc taglib_nif_funcs[] =
{
    {"new", 1, taglib_nif_resource_create},
    {"tag_title", 1, tag_title},
    {"tag_artist", 1, tag_artist},
    {"tag_album", 1, tag_album},
    {"tag_genre", 1, tag_genre},
    {"tag_disc", 1, tag_disc},
    {"tag_track", 1, tag_track},
    {"tag_year", 1, tag_year},
    {"tag_compilation", 1, tag_compilation},
    {"audio_length", 1, audio_length},
    {"artwork_picture", 1, artwork_picture}
};

ERL_NIF_INIT(Elixir.Taglib, taglib_nif_funcs, &taglib_nif_init, NULL, NULL, NULL)
