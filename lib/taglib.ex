defmodule Taglib do
  @moduledoc """
  Elixir bindings for Taglib.

  This module provides a simple wrapper around the Taglib C++ API.

      iex> {:ok, ref} = Taglib.new("song.mp3")
      {:ok, ""}
      iex> Taglib.tag_title(ref)
      "Mi Mujer"
      iex> Taglib.audio_length(ref)
      438
  """

  @on_load :load_nif

  @nif_path Path.join(:code.priv_dir(:taglib), "taglib_nif")

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
  def new(_filepath), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the title for the given `fileref`.
  """
  def tag_title(_fileref), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the artist for the given `fileref`.
  """
  def tag_artist(_fileref), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the album for the given `fileref`.
  """
  def tag_album(_fileref), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the genre for the given `fileref`.
  """
  def tag_genre(_fileref), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the year for the given `fileref`.
  """
  def tag_year(_fileref), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the track number for the given `fileref`.
  """
  def tag_track(_fileref), do: raise Code.LoadError, file: @nif_path

  @doc """
  Returns the audio length for the given `fileref`.
  """
  def audio_length(_fileref), do: raise Code.LoadError, file: @nif_path
end
