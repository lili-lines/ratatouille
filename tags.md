---
layout: page
title: Tags
permalink: /tags/
---

{%- assign sorted_tags = site.tags | sort -%}
{%- if sorted_tags.size == 0 -%}
No tags yet.
{%- else -%}
{%- for tag in sorted_tags -%}
<h2 id="{{ tag[0] | slugify }}">#{{ tag[0] }}</h2>
<ul>
  {%- for post in tag[1] -%}
  <li>
    <a href="{{ post.url | relative_url }}">{{ post.title | escape }}</a>
    <small>— {{ post.date | date: "%b %-d, %Y" }}</small>
  </li>
  {%- endfor -%}
</ul>
{%- endfor -%}
{%- endif -%}
