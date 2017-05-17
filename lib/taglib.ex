defmodule Taglib do
  @moduledoc """
  Elixir bindings for Taglib.

  This module provides a simple wrapper around the Taglib C++ API.

  ## Examples

      iex> {:ok, ref} = Taglib.new("song.mp3")
      {:ok, ""}
      iex> Taglib.tag_title(ref)
      "Mi Mujer"
      iex> Taglib.audio_length(ref)
      438
  """

  @on_load :load_nif

  @nif_path Path.join(:code.priv_dir(:taglib), "taglib_nif")

  @type fileref :: term
  @type mimetype :: String.t

  require Logger

  @doc false
  def load_nif do
    case :erlang.load_nif(@nif_path, 0) do
      :ok -> :ok
      {:error, {:load_failed, error}} -> Logger.error error
    end
  end

  @doc """
  Create a new file reference for the given `filepath`.
  """
  @spec new(Path.t) :: {:ok, fileref} | {:error, :invalid_file}
  def new(_path), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the title of the song.
  """
  @spec tag_title(fileref) :: String.t
  def tag_title(_ref), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the artist name of the song.
  """
  @spec tag_artist(fileref) :: String.t
  def tag_artist(_ref), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the album name of the song.
  """
  @spec tag_album(fileref) :: String.t
  def tag_album(_ref), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the genre of the song.
  """
  @spec tag_genre(fileref) :: String.t
  def tag_genre(_ref), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the disc number of the song.
  """
  @spec tag_disc(fileref) :: Integer.t
  def tag_disc(_ref), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the track number of the song.
  """
  @spec tag_track(fileref) :: Integer.t
  def tag_track(_ref), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the year of the song.
  """
  @spec tag_year(fileref) :: Integer.t
  def tag_year(_ref), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns `true` if the track is part of a compilation; elsewhise returns `false`.
  """
  @spec tag_compilation(fileref) :: boolean
  def tag_compilation(_ref), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the audio length of the song.
  """
  @spec audio_length(fileref) :: Integer.t
  def audio_length(_ref), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the cover-art picture of the song.
  """
  @spec artwork_picture(fileref) :: {mimetype, binary}
  def artwork_picture(_ref), do: raise Code.LoadError, file: @nif_path
end
