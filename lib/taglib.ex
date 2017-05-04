defmodule Taglib do
  @moduledoc """
  Elixir NIF bindings for Taglib.
  """

  @on_load :load_nif

  @nif_path "priv/taglib_nif"

  require Logger

  def load_nif do
    case :erlang.load_nif(@nif_path, 0) do
      :ok -> :ok
      {:error, {:load_failed, error}} -> Logger.error error
    end
  end

  def new(_filepath),         do: raise Code.LoadError, file: @nif_path
  def tag_title(_fileref),    do: raise Code.LoadError, file: @nif_path
  def tag_artist(_fileref),   do: raise Code.LoadError, file: @nif_path
  def tag_album(_fileref),    do: raise Code.LoadError, file: @nif_path
  def tag_genre(_fileref),    do: raise Code.LoadError, file: @nif_path
  def tag_year(_fileref),     do: raise Code.LoadError, file: @nif_path
  def tag_track(_fileref),    do: raise Code.LoadError, file: @nif_path
  def audio_length(_fileref), do: raise Code.LoadError, file: @nif_path
end
