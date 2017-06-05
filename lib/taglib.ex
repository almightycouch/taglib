defmodule Taglib do
  @moduledoc """
  Elixir bindings for Taglib.

  This module provides a simple wrapper around the Taglib C++ API.

  ## Examples

      iex> {:ok, t} = Taglib.new("song.mp3")
      {:ok, #Taglib<0.0.7.1219>}
      iex> Taglib.title(t)
      "Mi Mujer"
      iex> Taglib.duration(t)
      438
  """

  defstruct [:ptr, :ref]

  @on_load :load_nif

  @nif_path Path.join(:code.priv_dir(:taglib), "taglib_nif")

  @type mimetype :: String.t
  @type t :: %__MODULE__{
    ref: reference,
    ptr: binary
  }

  require Logger

  @doc false
  def load_nif do
    case :erlang.load_nif(@nif_path, 0) do
      :ok -> :ok
      {:error, {:load_failed, error}} -> Logger.error error
    end
  end

  @doc false
  def init(_path), do: raise Code.LoadError, file: @nif_path

  @doc """
  Create a new file reference for the given `path`.
  """
  @spec new(Path.t) :: {:ok, t} | {:error, term}
  def new(path) do
    case init(path) do
      {:ok, ptr} ->
        {:ok, struct(__MODULE__, ptr: ptr, ref: make_ref())}
      {:error, reason} ->
        {:error, reason}
    end
  end

  @doc """
  Returns the title of the song.
  """
  @spec title(t) :: String.t
  def title(taglib), do: tag_title(taglib.ptr)

  @doc false
  def tag_title(_ptr), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the artist name of the song.
  """
  @spec artist(t) :: String.t
  def artist(taglib), do: tag_artist(taglib.ptr)

  @doc false
  def tag_artist(_ptr), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the album name of the song.
  """
  @spec album(t) :: String.t
  def album(taglib), do: tag_album(taglib.ptr)

  @doc false
  def tag_album(_ptr), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the genre of the song.
  """
  @spec genre(t) :: String.t
  def genre(taglib), do: tag_genre(taglib.ptr)

  @doc false
  def tag_genre(_ptr), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the disc number of the song.
  """
  @spec disc(t) :: Integer.t
  def disc(taglib), do: tag_disc(taglib.ptr)

  @doc false
  def tag_disc(_ptr), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the track number of the song.
  """
  @spec track(t) :: Integer.t
  def track(taglib), do: tag_track(taglib.ptr)

  @doc false
  def tag_track(_ptr), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the year of the song.
  """
  @spec year(t) :: Integer.t
  def year(taglib), do: tag_year(taglib.ptr)

  @doc false
  def tag_year(_ptr), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns `true` if the track is part of a compilation; elsewhise returns `false`.
  """
  @spec compilation(t) :: boolean
  def compilation(taglib), do: tag_compilation(taglib.ptr)

  @doc false
  def tag_compilation(_ptr), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the audio length of the song.
  """
  @spec duration(t) :: Integer.t
  def duration(taglib), do: audio_length(taglib.ptr)

  @doc false
  def audio_length(_ptr), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the cover-art picture of the song.
  """
  @spec artwork(t) :: {mimetype, binary}
  def artwork(taglib), do: artwork_picture(taglib.ptr)

  @doc false
  def artwork_picture(_ptr), do: raise Code.LoadError, file: @nif_path
end

defimpl Inspect, for: Taglib do
  def inspect(taglib, _opts) do
    String.replace("#{inspect taglib.ref}", "Reference", "Taglib")
  end
end
