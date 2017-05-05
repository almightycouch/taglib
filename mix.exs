defmodule Taglib.Mixfile do
  use Mix.Project

  @version "0.1.0"

  def project do
    [app: :taglib,
     name: "Taglib",
     version: @version,
     elixir: "~> 1.4",
     package: package(),
     description: description(),
     compilers: [:elixir_make] ++ Mix.compilers,
     build_embedded: Mix.env == :prod,
     start_permanent: Mix.env == :prod,
     docs: docs(),
     deps: deps()]
  end

  def application do
    [extra_applications: [:logger]]
  end

  defp package do
    [files: ["c_src", "lib", "mix.exs", "Makefile", "README.md", "LICENSE"],
      maintainers: ["Mario Flach"],
      licenses: ["MIT"],
      links: %{"GitHub" => "https://github.com/almightycouch/taglib"}]
  end

  defp description do
    "Taglib bindings for Elixir"
  end

  defp docs do
    [extras: ["README.md"],
      main: "readme",
      source_ref: "v#{@version}",
      source_url: "https://github.com/almightycouch/taglib"]
  end

  defp deps do
    [{:elixir_make, "~> 0.4", runtime: false}]
  end
end
