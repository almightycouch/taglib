# Taglib

[![Travis](https://img.shields.io/travis/almightycouch/taglib.svg)](https://travis-ci.org/almightycouch/taglib)
[![Hex.pm](https://img.shields.io/hexpm/v/taglib.svg)](https://hex.pm/packages/taglib)
[![Documentation Status](https://img.shields.io/badge/docs-hexdocs-blue.svg)](http://hexdocs.pm/taglib)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/almightycouch/taglib/master/LICENSE)
[![Github Issues](https://img.shields.io/github/issues/almightycouch/taglib.svg)](http://github.com/almightycouch/taglib/issues)

Taglib bindings for Elixir.

## Documentation

See the [online documentation](https://hexdocs.pm/taglib/) for more information.

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
  [{:taglib, "~> 0.2"}]
end
```

## Usage

```elixir
iex> {:ok, t} = Taglib.new("song.mp3")
{:ok, #Taglib<0.0.7.1219>}
iex> Taglib.title(t)
"Mi Mujer"
iex> Taglib.duraiton(t)
438
```
