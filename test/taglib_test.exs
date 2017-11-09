defmodule TaglibTest do
  use ExUnit.Case

  test "reads tags from id3v2.mp3" do
    assert {:ok, t} = Taglib.new("test/samples/id3v2.mp3")
    assert Taglib.title(t)        == "Home"
    assert Taglib.artist(t)       == "Explosions In The Sky/Another/And Another"
    assert Taglib.album(t)        == "Friday Night Lights [Original Movie Soundtrack]"
    assert Taglib.genre(t)        == "Soundtrack"
    assert Taglib.duration(t)     == 0
    assert Taglib.disc(t)         == 1
    assert Taglib.track(t)        == 5
    assert Taglib.compilation(t)  == false
    assert {"image/jpg", _jpeg}   = Taglib.artwork(t)
  end
end
