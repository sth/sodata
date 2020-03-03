# Changelog
{{ range .Versions }}
<a name="{{ .Tag.Name }}"></a>
## [{{.Tag.Name}}]({{ $.Info.RepositoryURL }}/tree/{{ .Tag.Name }}),{{ if ne .Tag.Name "master" }} {{ datetime "2006-01-02" .Tag.Date }}{{ end }}{{ if .Tag.Previous }} ([compare to {{ .Tag.Previous.Name }}]({{ $.Info.RepositoryURL }}/compare/{{ .Tag.Previous.Name }}...{{ .Tag.Name }})){{ end }}

{{ range .Commits -}}
* {{ .Header }} ([{{ .Hash.Short }}]({{ $.Info.RepositoryURL }}/commit/{{ .Hash.Long }}))
{{ end }}

{{- if .NoteGroups -}}
{{ range .NoteGroups -}}
### {{ .Title }}

{{ range .Notes }}
{{ .Body }}
{{ end }}
{{ end -}}
{{ end -}}
{{ end -}}
