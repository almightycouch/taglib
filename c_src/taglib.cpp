#include <cstring>
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
#include <taglib/asffile.h>

#ifndef nullptr
#define nullptr NULL
#endif

static ErlNifResourceType* taglib_nif_resource = nullptr;

typedef struct
{
    TagLib::File *taglib_file;
} taglib_nif_handle;

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

static ERL_NIF_TERM tag_year(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if(TagLib::Tag *tag = taglib_nif_resource_tag(env, argc, argv)) {
        return enif_make_uint(env, tag->year());
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

static ERL_NIF_TERM audio_length(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if(TagLib::AudioProperties *props = taglib_nif_resource_audio_props(env, argc, argv)) {
        return enif_make_int(env, props->length());
    } else {
        return enif_make_badarg(env);
    }
}

static ERL_NIF_TERM picture(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if(TagLib::File *file = taglib_nif_resource_file(env, argc, argv)) {
        if(TagLib::MP4::File *mp4 = dynamic_cast<TagLib::MP4::File *>(file)) {
            TagLib::MP4::Tag *tag = mp4->tag();
            TagLib::MP4::ItemListMap items = tag->itemListMap();
            TagLib::MP4::CoverArtList covers = items["covr"].toCoverArtList();
            if(!covers.isEmpty()) {
                TagLib::MP4::CoverArt cover = covers.front();
                return string_to_binary(env, cover.data());
            }
        } else if(TagLib::MPEG::File *mp3 = dynamic_cast<TagLib::MPEG::File *>(file)) {
            TagLib::ID3v2::Tag *tag = mp3->ID3v2Tag();
            TagLib::ID3v2::FrameList covers = tag->frameList("APIC");
            if(!covers.isEmpty()) {
                TagLib::ID3v2::AttachedPictureFrame *frame = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(covers.front());
                return string_to_binary(env, frame->picture());
            }
        } else if(TagLib::FLAC::File *flac = dynamic_cast<TagLib::FLAC::File *>(file)) {
            TagLib::List<TagLib::FLAC::Picture *> covers = flac->pictureList();
            if(!covers.isEmpty()) {
                TagLib::FLAC::Picture *cover = covers.front();
                return string_to_binary(env, cover->data());
            }
        } else if(TagLib::ASF::File *asf = dynamic_cast<TagLib::ASF::File *>(file)) {
            TagLib::ASF::Tag *tag = static_cast<TagLib::ASF::Tag *>(asf->tag());
            TagLib::ASF::AttributeListMap attrs = tag->attributeListMap();
            TagLib::ASF::AttributeList covers = attrs["WM/Picture"];
            if(!covers.isEmpty()) {
                TagLib::ASF::Picture cover = covers.front().toPicture();
                return string_to_binary(env, cover.picture());
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
    {"tag_year", 1, tag_year},
    {"tag_track", 1, tag_track},
    {"audio_length", 1, audio_length},
    {"picture", 1, picture}
};

ERL_NIF_INIT(Elixir.Taglib, taglib_nif_funcs, &taglib_nif_init, NULL, NULL, NULL)
