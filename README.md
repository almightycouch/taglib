# Taglib

Taglib bindings for Elixir.

## Installation

First, ensure you have the [TagLib](http://taglib.org) libraries installed on your system:

#### OSX
```bash
brew install taglib
```

#### Ubuntu
```bash
sudo apt-get install libtag1-dev
```

Next, add `:taglib` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [{:taglib, "~> 0.1"}]
end
```

## Usage

```elixir
iex> {:ok, ref} = Taglib.new("song.mp3")
{:ok, ""}
iex> Taglib.tag_title(ref)
"Mi Mujer"
iex> Taglib.audio_length(ref)
438
```
