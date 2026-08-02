CREATE TABLE [dbo].[00_tehniki](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [nomer] [int] NULL,
    [otv] [nvarchar](50) NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[01_teploprovodnost_grunta](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [tpground] [float] NULL,
    [ground] [nvarchar](50) NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[02_teplootdacha_ot_izolyatsii_k_okruzhaemomu_vozduhu](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [tip] [nvarchar](50) NULL,
    [min_izoloutair] [int] NULL,
    [max_izoloutair] [int] NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[06_sostoyanie_izolyatsii](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [kod_k_izol] [int] NULL,
    [k_izol_min] [float] NULL,
    [k_izol_max] [float] NULL,
    [sosizol] [nvarchar](150) NULL,
    [name_sostizol] [nvarchar](30) NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[07_teploizolyatsionnyy_material](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [kod_izol] [int] NULL,
    [name] [nvarchar](100) NULL,
    [l] [float] NULL,
    [k] [float] NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[08_mestnye_gidravlicheskie_soprotivleniya](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [name_rm] [nvarchar](70) NULL,
    [mestnoe_min] [float] NULL,
    [mestnoe_max] [float] NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[09_spisok_ulits](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [street] [nvarchar](50) NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[10_teplovye_harakteristiki_zdaniy](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [nomer] [int] NULL,
    [sategory] [nvarchar](50) NULL,
    [name_zd] [nvarchar](50) NULL,
    [tr_tv_co] [int] NULL,
    [vmin] [float] NULL,
    [vmax] [float] NULL,
    [pr_year] [nvarchar](1) NULL,
    [q_ot] [float] NULL,
    [q_vt] [float] NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[11_material_pokrovnogo_sloya_izolyatsii](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [kod] [nvarchar](5) NULL,
    [znachenie] [nvarchar](255) NULL,
    [tolschina__mm] [nvarchar](255) NULL,
    [primechanie] [nvarchar](255) NULL,
    [gost] [nvarchar](255) NULL,
    [marki] [nvarchar](255) NULL,
    [goryuchest] [nvarchar](255) NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[12_materialy_konstruktsiy_kamer](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [mat_construct] [nvarchar](50) NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[13_klimaticheskie_dannye](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [sity] [nvarchar](255) NULL,
    [tag] [int] NULL,
    [otopl_pr] [float] NULL,
    [vetn_pr] [float] NULL,
    [t_sr] [float] NULL,
    [t_sr_min] [float] NULL,
    [t_1month] [float] NULL,
    [t_2month] [float] NULL,
    [t_3month] [float] NULL,
    [t_4month] [float] NULL,
    [t_5month] [float] NULL,
    [t_6month] [float] NULL,
    [t_7month] [float] NULL,
    [t_8month] [float] NULL,
    [t_9month] [float] NULL,
    [t_10month] [float] NULL,
    [t_11month] [float] NULL,
    [t_12month] [float] NULL,
    [t_year] [float] NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[14_sherohovatost_stalnyh_trub](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [tip] [nvarchar](50) NULL,
    [s_min] [float] NULL,
    [s_max] [float] NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [d] [float] NULL,
    [s_ton] [int] NULL,
    [s_kan] [int] NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[18_fizicheskie_svoystva_vody](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [t] [float] NULL,
    [p] [float] NULL,
    [g] [float] NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [r] [float] NULL,
    [15] [nvarchar](255) NULL,
    [20] [nvarchar](255) NULL,
    [25] [nvarchar](255) NULL,
    [32] [nvarchar](255) NULL,
    [100] [nvarchar](255) NULL,
    [70] [nvarchar](255) NULL,
    [125] [nvarchar](255) NULL,
    [150] [nvarchar](255) NULL,
    [184] [nvarchar](255) NULL,
    [207] [nvarchar](255) NULL,
    [259] [nvarchar](255) NULL,
    [300] [nvarchar](255) NULL,
    [359] [nvarchar](255) NULL,
    [408] [nvarchar](255) NULL,
    [414] [nvarchar](255) NULL,
    [466] [nvarchar](255) NULL,
    [514] [nvarchar](255) NULL,
    [612] [nvarchar](255) NULL,
    [700] [nvarchar](255) NULL,
    [800] [nvarchar](255) NULL,
    [898] [nvarchar](255) NULL,
    [996] [nvarchar](255) NULL,
    [1096] [nvarchar](255) NULL,
    [1192] [nvarchar](255) NULL,
    [1392] [nvarchar](255) NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[23_popravochnyy_koeff_rascheta_otopitelnoy_nagruzki_zdaniya](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [nomer] [int] NULL,
    [tn] [float] NULL,
    [alfa] [float] NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [nomer] [int] NULL,
    [typ_co] [nvarchar](50) NULL,
    [delta_t] [nvarchar](50) NULL,
    [q_udel] [float] NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[26_mesto_prokladki_uchastka_truboprovoda](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [mesto_prokladki] [nvarchar](50) NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[27_vid_pokrytiya_uchastka_teploprovoda](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [vid_pokrit] [nvarchar](50) NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[28_koeffitsienty_rascheta_balansovoy_nagruzki_gv](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [pr] [float] NULL,
    [sm] [float] NULL,
    [ps] [float] NULL,
    [pw] [float] NULL,
    [o] [float] NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [name_zd] [nvarchar](50) NULL,
    [populace] [int] NULL,
    [gvs_max] [float] NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[30_koeffitsienty_mestnyh_teplovyh_poter](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [pr] [nvarchar](50) NULL,
    [diametr] [float] NULL,
    [beta_mag] [float] NULL,
    [beta_rasp] [float] NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[31_udelnyy_obem_vody_v_sisteme_ventilyatsii](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [nomer] [int] NULL,
    [typ_co] [nvarchar](50) NULL,
    [delta_t] [nvarchar](50) NULL,
    [q_udel] [float] NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[36_naimenovanie_rayona_mestonahozhdeniya_teploprovoda](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [name_typ] [nvarchar](50) NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[39_normy_teplovyh_poter](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [d] [float] NULL,
    [dy] [float] NULL,
    [date] [int] NULL,
    [proklad] [nvarchar](50) NULL,
    [tg] [float] NULL,
    [tn] [int] NULL,
    [t2] [float] NULL,
    [t1_1] [float] NULL,
    [t1_2] [float] NULL,
    [t1_3] [float] NULL,
    [t1_4] [float] NULL,
    [qp_1] [float] NULL,
    [qo_1] [float] NULL,
    [qp_2] [float] NULL,
    [qo_2] [float] NULL,
    [qp_3] [float] NULL,
    [qo_3] [float] NULL,
    [qp_4] [float] NULL,
    [qo_4] [float] NULL,
    [qp_1gt5000] [float] NULL,
    [qo_1gt5000] [float] NULL,
    [qp_2gt5000] [float] NULL,
    [qo_2gt5000] [float] NULL,
    [qp_3gt5000] [float] NULL,
    [qo_3gt5000] [float] NULL,
    [qp_4gt5000] [float] NULL,
    [qo_4gt5000] [float] NULL,
    mode bit NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [t_r] [float] NULL,
    [alfa] [float] NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[41_otsenka_faktorov_otritsatelnyh_vozdeystviy_na_truboprovod](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [nomer] [int] NULL,
    [date] [date] NULL,
    [f_kol_let_lt7] [float] NULL,
    [f_kol_let_lt15] [float] NULL,
    [f_kol_let_lt20] [float] NULL,
    [f_kol_let_lt25] [float] NULL,
    [f_kol_let_gt25] [float] NULL,
    [f_koef_povrlt1] [float] NULL,
    [f_koef_povrlt2] [float] NULL,
    [f_koef_povrlt4] [float] NULL,
    [f_koef_povrlt5] [float] NULL,
    [f_koef_povrgt5] [float] NULL,
    [f_koef_povr_2godalt1] [float] NULL,
    [f_koef_povr_2godalt2] [float] NULL,
    [f_koef_povr_2godalt4] [float] NULL,
    [f_koef_povr_2godalt5] [float] NULL,
    [f_koef_povr_2godagt5] [float] NULL,
    [f_10_elektro] [float] NULL,
    [f_5_elektro] [float] NULL,
    [f_3_elektro] [float] NULL,
    [f_1_elektro] [float] NULL,
    [f_0_elektro] [float] NULL,
    [f_peres_elektro] [float] NULL,
    [f_10_gas] [float] NULL,
    [f_5_gas] [float] NULL,
    [f_3_gas] [float] NULL,
    [f_1_gas] [float] NULL,
    [f_0_gas] [float] NULL,
    [f_peres_gas] [float] NULL,
    [f_10_transp] [float] NULL,
    [f_5_transp] [float] NULL,
    [f_3_transp] [float] NULL,
    [f_1_transp] [float] NULL,
    [f_0_transp] [float] NULL,
    [f_peres_transp] [float] NULL,
    [f_elektropotenzlt0] [float] NULL,
    [f_elektropotenzlt04] [float] NULL,
    [f_elektropotenzgt04] [float] NULL,
    [f_nar_korroz] [nvarchar](50) NULL,
    [f_vnu_korroz] [nvarchar](50) NULL,
    [f_grund_water] [float] NULL,
    [f_pavod_water] [float] NULL,
    [f_vod_water] [float] NULL,
    [f_fek_water] [float] NULL,
    [f_st_stik] [tinyint] NULL,
    [f_st_islom] [tinyint] NULL,
    [f_st_otkl] [tinyint] NULL,
    [f_st_nes] [tinyint] NULL,
    [f_st_opor] [tinyint] NULL,
    [f_water_otved] [nvarchar](50) NULL,
    [f_otkl] [nvarchar](50) NULL,
    [f_okl_pot] [nvarchar](50) NULL,
    [f_slog_arbeit] [nvarchar](50) NULL,
    [f_tratuar] [nvarchar](50) NULL,
    [f_doroga] [nvarchar](50) NULL,
    [f_ucherb_narod] [nvarchar](50) NULL,
    [f_ucherb_gorod] [nvarchar](50) NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[42_koeffitsienty_opredeleniya_tehnologicheskih_psv](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [kod] [int] NULL,
    [name_psv] [nvarchar](50) NULL,
    [kratnost_psv] [float] NULL,
    [help] [nvarchar](120) NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[43_teplovaya_akkumulyatsiya_zdaniy](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [nomer] [int] NULL,
    [harakter] [nvarchar](100) NULL,
    [serija] [nvarchar](50) NULL,
    [utepl] [nvarchar](100) NULL,
    [tol_stena] [nvarchar](70) NULL,
    [tol_izol] [nvarchar](70) NULL,
    [help] [nvarchar](100) NULL,
    [pomechenie] [nvarchar](50) NULL,
    [k_akkuml] [float] NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[44_temp_padeniya_temperatury_vnutri_zdaniy](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [k_akkuml] [int] NULL,
    [t_0] [float] NULL,
    [t_10] [float] NULL,
    [t_20] [float] NULL,
    [t_30] [float] NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[45_koeffitsient_nachala_kavitatsii](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [nomer] [int] NULL,
    [po] [nvarchar](50) NULL,
    [k_kavit] [float] NULL,
    [primech] [nvarchar](50) NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[46_kvartaly_raboty_masterov](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [nomer] [int] NULL,
    [master] [int] NULL,
    [name_kv] [nvarchar](50) NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[edinitsy_aleksey](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [stand_razmer] [nvarchar](20) NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[edinitsy_izmereniya](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [kodvelichini] [int] NULL,
    [razmer] [nvarchar](30) NULL,
    [k_razmer] [float] NULL,
    [velichina] [nvarchar](50) NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[edinitsypolzovatelya](
    [id] [int] IDENTITY(1,1) NOT NULL,
    [velichina] [nvarchar](50) NULL,
    [kodvelichini] [int] NULL,
    [stand_razmer] [nvarchar](50) NULL,
PRIMARY KEY CLUSTERED 
(
    [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
SET IDENTITY_INSERT [dbo].[00_tehniki] ON 

INSERT [dbo].[00_tehniki] ([id], [nomer], [otv]) VALUES (1, 1, N'Иванов')
INSERT [dbo].[00_tehniki] ([id], [nomer], [otv]) VALUES (2, 2, N'Петров')
SET IDENTITY_INSERT [dbo].[00_tehniki] OFF

GO

SET IDENTITY_INSERT [dbo].[01_teploprovodnost_grunta] ON 

INSERT [dbo].[01_teploprovodnost_grunta] ([id], [tpground], [ground]) VALUES (1, 1.1, N'Песок (супесь), сухой')
INSERT [dbo].[01_teploprovodnost_grunta] ([id], [tpground], [ground]) VALUES (2, 1.92, N'Песок (супесь), влажный')
INSERT [dbo].[01_teploprovodnost_grunta] ([id], [tpground], [ground]) VALUES (3, 2.44, N'Песок (супесь), водонасыщенный')
INSERT [dbo].[01_teploprovodnost_grunta] ([id], [tpground], [ground]) VALUES (4, 1.74, N'Глина (суглинок), сухая')
INSERT [dbo].[01_teploprovodnost_grunta] ([id], [tpground], [ground]) VALUES (5, 2.56, N'Глина (суглинок), влажная')
INSERT [dbo].[01_teploprovodnost_grunta] ([id], [tpground], [ground]) VALUES (6, 2.67, N'Глина (суглинок), водонасыщенная')
INSERT [dbo].[01_teploprovodnost_grunta] ([id], [tpground], [ground]) VALUES (7, 2.03, N'Гравий (щебень), сухой')
INSERT [dbo].[01_teploprovodnost_grunta] ([id], [tpground], [ground]) VALUES (8, 2.73, N'Гравий (щебень), влажный')
INSERT [dbo].[01_teploprovodnost_grunta] ([id], [tpground], [ground]) VALUES (9, 3.37, N'Гравий (щебень), водонасыщенный')
SET IDENTITY_INSERT [dbo].[01_teploprovodnost_grunta] OFF
GO

SET IDENTITY_INSERT [dbo].[02_teplootdacha_ot_izolyatsii_k_okruzhaemomu_vozduhu] ON 

INSERT [dbo].[02_teplootdacha_ot_izolyatsii_k_okruzhaemomu_vozduhu] ([id], [tip], [min_izoloutair], [max_izoloutair]) VALUES (1, N'Вертикальный трубопровод', 6, 11)
INSERT [dbo].[02_teplootdacha_ot_izolyatsii_k_okruzhaemomu_vozduhu] ([id], [tip], [min_izoloutair], [max_izoloutair]) VALUES (2, N'Горизонтальный трубопровод', 6, 10)
SET IDENTITY_INSERT [dbo].[02_teplootdacha_ot_izolyatsii_k_okruzhaemomu_vozduhu] OFF

GO

SET IDENTITY_INSERT [dbo].[06_sostoyanie_izolyatsii] ON 

INSERT [dbo].[06_sostoyanie_izolyatsii] ([id], [kod_k_izol], [k_izol_min], [k_izol_max], [sosizol], [name_sostizol]) VALUES (1, 1, 1.3, 1.5, N'Незначительное нарушение покровного и основного слоя изоляционной конструкции', N'Нарушение <30%')
INSERT [dbo].[06_sostoyanie_izolyatsii] ([id], [kod_k_izol], [k_izol_min], [k_izol_max], [sosizol], [name_sostizol]) VALUES (2, 3, 1.7, 2.1, N'Частичное разрушение теплоизоляционной конструкции, уплотнение основного слоя изоляции на 30-50%', N'Нарушение на 30-50%')
INSERT [dbo].[06_sostoyanie_izolyatsii] ([id], [kod_k_izol], [k_izol_min], [k_izol_max], [sosizol], [name_sostizol]) VALUES (3, 5, 1.4, 1.6, N'Незначительное увлажнение изоляции (10-15%)', N'Увлажнение 10-15%')
INSERT [dbo].[06_sostoyanie_izolyatsii] ([id], [kod_k_izol], [k_izol_min], [k_izol_max], [sosizol], [name_sostizol]) VALUES (4, 7, 3, 4.5, N'Сильное увлажнение изоляции (40-60%)', N'Увлажнение 40-60%')
INSERT [dbo].[06_sostoyanie_izolyatsii] ([id], [kod_k_izol], [k_izol_min], [k_izol_max], [sosizol], [name_sostizol]) VALUES (5, 9, 1.6, 1.8, N'Уплотнение изоляции сверху трубопровода и обвисание снизу', N'Обвисание')
INSERT [dbo].[06_sostoyanie_izolyatsii] ([id], [kod_k_izol], [k_izol_min], [k_izol_max], [sosizol], [name_sostizol]) VALUES (6, 11, 3.5, 3.5, N'Уплотнение основного слоя на 70%', N'Уплотнение на 70%')
INSERT [dbo].[06_sostoyanie_izolyatsii] ([id], [kod_k_izol], [k_izol_min], [k_izol_max], [sosizol], [name_sostizol]) VALUES (7, 13, 1.9, 2.6, N'Увлажнение изоляции на 20-30%', N'Увлажнение 20-30%')
INSERT [dbo].[06_sostoyanie_izolyatsii] ([id], [kod_k_izol], [k_izol_min], [k_izol_max], [sosizol], [name_sostizol]) VALUES (8, 15, 3, 5, N'Периодическое затопление грунтовыми водами или смежными коммуникациями', N'Затопление')
SET IDENTITY_INSERT [dbo].[06_sostoyanie_izolyatsii] OFF

GO

SET IDENTITY_INSERT [dbo].[07_teploizolyatsionnyy_material] ON 

INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (1, 1, N'Асбестовый матрац, заполненный совелитом', 0.087, 0.00012)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (2, 2, N'Асбестовый матрац, заполненный стекловолокном', 0.058, 0.00023)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (3, 3, N'Асботкань в несколько слоев', 0.13, 0.00026)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (4, 4, N'Асбестовый шнур', 0.12, 0.00031)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (5, 5, N'Асбестовый шнур (ШАОН)', 0.13, 0.00026)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (6, 6, N'Асбопухшнур (ШАП)', 0.093, 0.0002)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (7, 7, N'Асбовермикулитовые изделия марки 250', 0.081, 0.00023)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (8, 8, N'Асбовермикулитовые изделия марки 300', 0.087, 0.00023)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (9, 9, N'Битумоперлит', 0.12, 0.00023)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (10, 10, N'Битумокерамзит', 0.13, 0.00023)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (11, 11, N'Битумовермикулит', 0.13, 0.00023)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (12, 12, N'Вулканитовые плиты марки 300', 0.074, 0.00015)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (13, 13, N'Диатомовые изделия марки 500', 0.116, 0.00023)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (14, 14, N'Диатомовые изделия марки 600', 0.14, 0.00023)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (15, 15, N'Известково-кремнеземные изделия марки 200', 0.069, 0.00015)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (16, 16, N'Маты минераловатные прошивные марки 100', 0.045, 0.0002)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (17, 17, N'Маты минераловатные прошивные марки 125', 0.049, 0.0002)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (18, 18, N'Маты из минеральной ваты марки 75', 0.043, 0.00022)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (19, 19, N'Маты из непрерывного стекловолокна', 0.04, 0.00026)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (20, 20, N'Маты стекловатные марки 50', 0.042, 0.00028)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (21, 21, N'Пенобетонные изделия', 0.11, 0.0003)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (22, 22, N'Пенопласт ФРП-1 группы 100', 0.043, 0.00019)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (23, 23, N'Пенополимербетон', 0.07, 0)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (24, 24, N'Пенополиуретан', 0.05, 0)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (25, 25, N'Перлитоцементные изделия марки 300', 0.076, 0.000185)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (26, 26, N'Перлитоцементные изделия марки 350', 0.081, 0.000185)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (27, 27, N'Плиты из минеральной ваты марки 75', 0.043, 0.00022)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (28, 28, N'Плиты стекловатные марки 50', 0.042, 0.00028)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (29, 29, N'Плиты минераловатные полужесткие марки 100', 0.044, 0.00021)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (30, 30, N'Плиты минераловатные полужесткие марки 125', 0.047, 0.000185)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (31, 31, N'Плиты минераловатные марки 250', 0.056, 0.000185)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (32, 32, N'Плиты стекловатные полужесткие марки 75', 0.044, 0.00023)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (33, 33, N'Полуцилиндры минераловатные марки 150', 0.049, 0.0002)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (34, 34, N'Полуцилиндры минераловатные марки 200', 0.052, 0.000185)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (35, 35, N'Полосы из непрерывного стекловолокна', 0.04, 0.00026)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (36, 36, N'Совелитовые изделия марки 350', 0.076, 0.000185)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (37, 37, N'Совелитовые изделия марки 400', 0.078, 0.000185)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (38, 38, N'Скорлупы минераловатные оштукатуренные', 0.069, 0.00019)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (39, 39, N'Фенольный поропласт ФЛ монолит', 0.05, 0)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (40, 40, N'Шнур минераловатный марки 200', 0.056, 0.000185)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (41, 41, N'Шнур минераловатный марки 250', 0.058, 0.000185)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (42, 42, N'Шнур минераловатный марки 300', 0.061, 0.000185)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (43, 43, N'Цилиндры минераловатные марки 250', 0.056, 0.000185)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (44, 44, N'Цилиндры минераловатные марки 150', 0.049, 0.0002)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (45, 45, N'Цилиндры минераловатные марки 200', 0.052, 0.000185)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (46, 46, N'Термокраска', 0, 0)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (47, 47, N'Стекловата фольгированная', 0, 0)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (48, 48, N'Маты минераловатные', 0, 0)
INSERT [dbo].[07_teploizolyatsionnyy_material] ([id], [kod_izol], [name], [l], [k]) VALUES (49, 49, N'Минплита', 0, 0)
SET IDENTITY_INSERT [dbo].[07_teploizolyatsionnyy_material] OFF

GO

SET IDENTITY_INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ON 

INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (1, N'Вентили ''Косва''', 0.5, 1)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (2, N'Вентили проходные,d=50/400мм.', 4, 8)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (3, N'Вентиль прямоточного типа, d=200мм', 0.36, 18)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (4, N'Вентиль прямоточного типа, d=25-250мм', 0.32, 1.04)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (5, N'Вентиль прямоточного типа, d=38мм', 0.85, 12)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (6, N'Вентиль типа ''Рей''', 3.4, 3.4)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (7, N'Вентиль штампованный', 7.8, 7.8)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (8, N'Водоотделитель', 8, 12)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (9, N'Входные насадки', 1, 1)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (10, N'Входные насадки', 0.5, 1)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (11, N'Входные насадки с плавным изменением сечения', 0.3, 0.6)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (12, N'Грязевик', 4, 10)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (13, N'Задвижка клинкетная', 0.2, 0.2)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (14, N'Задвижка паровая с паровым затвором', 0.75, 0.75)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (15, N'Задвижка нормальная', 0.3, 0.5)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (16, N'Клапан всасывающий, d=40-750мм', 1.6, 12)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (17, N'Клапан обратный, d=40-750мм', 1.3, 2.9)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (18, N'Колена 90 гладкие R=2d', 0.7, 0.7)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (19, N'Колена 90 гладкие R=4d', 0.3, 0.3)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (20, N'Колена 90 гладкие R>4d', 0.05, 0.2)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (21, N'Колена 90 гнутые гладкие R=1.0d', 1, 1)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (22, N'Колено 22.5 сварное (1 шов)', 0.11, 0.11)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (23, N'Колено 45 сварное (1 шов)', 0.32, 0.32)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (24, N'Колено 45 сварное (2 шва)', 0.11, 0.11)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (25, N'Колено 60 сварное (1 шов)', 0.68, 0.68)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (26, N'Колено 60 сварное (2 шва)', 0.15, 0.15)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (27, N'Колено 60 сварное (3 шва)', 0.11, 0.11)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (28, N'Колено 90 сварное (1 шов)', 1.27, 1.27)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (29, N'Колено 90 сварное (2 шва)', 0.4, 0.4)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (30, N'Колено без ниши', 0.05, 3)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (31, N'Кран проходной', 0.6, 2)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (32, N'Кран угловой', 0.4, 0.4)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (33, N'Тройник (Встречный ток)', 3, 3)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (34, N'Угольник 90', 1, 2)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (35, N'Задвижка с распорным грибком и выдв. Трубой', 0.1, 0.1)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (36, N'Компенсатор волнистый', 2.5, 2.5)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (37, N'Компенсатор лировидный гладкий', 1.7, 1.7)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (38, N'Компенсатор лирообразн. из волн.трубы,d=50-500мм', 3, 5)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (39, N'Компенсатор лирообразный с фальцами,d=80-500мм', 2, 3)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (40, N'Компенсатор П-образный,d=50-500мм', 2, 2.9)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (41, N'Компенсатор сальниковый', 0.2, 0.3)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (42, N'Вентиль с косым шпинделем', 0.5, 0.5)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (43, N'Вентиль с вертикальным шпинделем', 6, 6)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (44, N'Клапан обратный нормальный', 7, 7)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (45, N'Клапан обратный "захлопка"', 3, 3)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (46, N'Компенсатор однолинзовый без рубашки', 0.5, 1.6)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (47, N'Компенсатор однолинзовый с рубашкой', 0.1, 0.1)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (48, N'Отводы гнутые под углом 90 Град, со складками R=3d', 0.8, 0.8)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (49, N'Отводы гнутые под углом 90 Град, со складками R=4d', 0.5, 0.5)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (50, N'Отводы гнутые под углом 90 Град, гладкие R=1d', 1, 1)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (51, N'Отводы гнутые под углом 90 Град, гладкие R=3d', 0.5, 0.5)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (52, N'Отводы гнутые под углом 90 Град, гладкие R=4d', 0.3, 0.3)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (53, N'Отводы сварные одношовные под углом 30 Град.', 0.2, 0.2)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (54, N'Отводы сварные одношовные под углом 45 Град.', 0.3, 0.3)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (55, N'Отводы сварные одношовные под углом 60 Град.', 0.7, 0.7)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (56, N'Отводы сварные двухшовные под углом 90 Град.', 0.6, 0.6)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (57, N'Отводы сварные трехшовные под углом 90 Град.', 0.5, 0.5)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (58, N'Тройник при слиянии потока, проход', 1.2, 1.2)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (59, N'Тройник при слиянии потока, ответвление', 1.8, 1.8)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (60, N'Тройник при разветвлении потока, проход', 1, 1)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (61, N'Тройник при разветвлении потока, ответвление', 1.5, 1.5)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (62, N'Тройник при встречном потоке', 3, 3)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (63, N'Внезапное разширение', 1, 1)
INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] ([id], [name_rm], [mestnoe_min], [mestnoe_max]) VALUES (64, N'Внезапное сужение', 0.5, 0.5)
SET IDENTITY_INSERT [dbo].[08_mestnye_gidravlicheskie_soprotivleniya] OFF

GO

SET IDENTITY_INSERT [dbo].[09_spisok_ulits] ON 

INSERT [dbo].[09_spisok_ulits] ([id], [street]) VALUES (1, N'Гоголя')
INSERT [dbo].[09_spisok_ulits] ([id], [street]) VALUES (2, N'Бульвар Мира')
INSERT [dbo].[09_spisok_ulits] ([id], [street]) VALUES (3, NULL)
SET IDENTITY_INSERT [dbo].[09_spisok_ulits] OFF
GO


SET IDENTITY_INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ON 

INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (1, 164, N'Промышленное', N'Котельная', 18, 2001, 5000, N' ', 0.1, 0.4)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (2, 165, N'Промышленное', N'Котельная', 18, 5001, 10000, N' ', 0.1, 0.3)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (3, 166, N'Промышленное', N'Котельная', 18, 10001, 1000000, N' ', 0.08, 0.2)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (4, 167, N'Промышленное', N'Мастерская', 18, 1, 10000, N' ', 0.5, 0.5)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (5, 168, N'Промышленное', N'Мастерская', 18, 10001, 15000, N' ', 0.4, 0.3)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (6, 169, N'Промышленное', N'Мастерская', 18, 15001, 20000, N' ', 0.35, 0.25)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (7, 170, N'Промышленное', N'Мастерская', 18, 20001, 1000000, N' ', 0.3, 0.2)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (8, 171, N'Промышленное', N'Насосная', 18, 1, 500, N' ', 1.05, 0.2)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (9, 172, N'Промышленное', N'Насосная', 18, 500, 1000, N' ', 1, 0.2)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (10, 173, N'Промышленное', N'Насосная', 18, 1001, 2000, N' ', 0.6, 0.2)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (11, 174, N'Промышленное', N'Насосная', 18, 2001, 1000000, N' ', 0.5, 0.2)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (12, 175, N'Промышленное', N'Компрессорная', 18, 1, 500, N' ', 0.7, 0.2)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (13, 176, N'Промышленное', N'Компрессорная', 18, 500, 1000, N' ', 0.45, 0.2)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (14, 177, N'Промышленное', N'Компрессорная', 18, 1001, 2000, N' ', 0.6, 0.2)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (15, 178, N'Промышленное', N'Компрессорная', 18, 2001, 5000, N' ', 0.4, 0.2)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (16, 179, N'Промышленное', N'Компрессорная', 18, 5001, 1000000, N' ', 0.35, 0.2)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (17, 180, N'Промышленное', N'Газгенераторная', 18, 1, 1000000, N' ', 0.1, 1.8)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (18, 181, N'Промышленное', N'Генерация масел', 18, 1, 1000000, N' ', 0.6, 0.5)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (19, 182, N'Промышленное', N'Склад химикатов', 18, 1, 1000, N' ', 0.85, 0.5)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (20, 183, N'Промышленное', N'Склад химикатов', 18, 1001, 2000, N' ', 0.75, 0.5)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (21, 184, N'Промышленное', N'Склад химикатов', 18, 2001, 1000000, N' ', 0.65, 0.6)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (22, 185, N'Промышленное', N'Склад моделей', 18, 1, 1000, N' ', 0.8, 0.6)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (23, 186, N'Промышленное', N'Склад моделей', 18, 1001, 2000, N' ', 0.7, 0.6)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (24, 187, N'Промышленное', N'Склад моделей', 18, 2001, 5000, N' ', 0.6, 0.6)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (25, 188, N'Промышленное', N'Склад моделей', 18, 5001, 1000000, N' ', 0.45, 0.6)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (26, 189, N'Промышленное', N'Административное', 18, 1, 500, N' ', 0.6, 0.6)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (27, 190, N'Промышленное', N'Административное', 18, 500, 1000, N' ', 0.45, 0.6)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (28, 191, N'Промышленное', N'Административное', 18, 1001, 2000, N' ', 0.4, 0.14)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (29, 192, N'Промышленное', N'Административное', 18, 2001, 5000, N' ', 0.33, 0.12)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (30, 193, N'Промышленное', N'Административное', 18, 5001, 10000, N' ', 0.3, 0.11)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (31, 194, N'Промышленное', N'Административное', 18, 10001, 1000000, N' ', 0.25, 0.1)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (32, 195, N'Промышленное', N'Вспомогательное', 18, 1, 500, N' ', 0.6, 0.6)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (33, 196, N'Промышленное', N'Вспомогательное', 18, 500, 1000, N' ', 0.45, 0.6)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (34, 197, N'Промышленное', N'Вспомогательное', 18, 1001, 2000, N' ', 0.4, 0.14)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (35, 198, N'Промышленное', N'Вспомогательное', 18, 2001, 5000, N' ', 0.33, 0.12)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (36, 199, N'Промышленное', N'Вспомогательное', 18, 5001, 10000, N' ', 0.3, 0.11)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (37, 106, N'Учреждение', N'Баня', 25, 1, 5000, N' ', 0.28, 1)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (38, 107, N'Учреждение', N'Баня', 25, 5001, 10000, N' ', 0.25, 0.95)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (39, 108, N'Учреждение', N'Баня', 25, 10001, 1000000, N' ', 0.23, 0.9)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (40, 109, N'Учреждение', N'Прачечная', 15, 1, 5000, N' ', 0.38, 0.8)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (41, 110, N'Учреждение', N'Прачечная', 15, 5001, 10000, N' ', 0.33, 0.78)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (42, 111, N'Учреждение', N'Прачечная', 15, 10001, 1000000, N' ', 0.31, 0.75)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (43, 112, N'Учреждение', N'Общепит', 16, 1, 5000, N' ', 0.35, 0.7)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (44, 113, N'Учреждение', N'Общепит', 16, 5001, 10000, N' ', 0.33, 0.65)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (45, 114, N'Учреждение', N'Общепит', 16, 10001, 1000000, N' ', 0.3, 0.6)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (46, 115, N'Учреждение', N'Лаборатория', 16, 1, 5000, N' ', 0.37, 1)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (47, 116, N'Учреждение', N'Лаборатория', 16, 5001, 10000, N' ', 0.35, 0.95)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (48, 117, N'Учреждение', N'Лаборатория', 16, 10001, 1000000, N' ', 0.33, 0.9)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (49, 118, N'Учреждение', N'Пожарка', 15, 1, 5000, N' ', 0.48, 0.14)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (50, 119, N'Учреждение', N'Пожарка', 15, 5001, 10000, N' ', 0.46, 0.09)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (51, 120, N'Учреждение', N'Пожарка', 15, 10001, 1000000, N' ', 0.45, 0.09)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (52, 121, N'Учреждение', N'Гараж', 10, 1, 2000, N' ', 0.7, 0.7)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (53, 122, N'Учреждение', N'Гараж', 10, 2001, 3000, N' ', 0.6, 0.7)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (54, 123, N'Учреждение', N'Гараж', 10, 3001, 5000, N' ', 0.55, 0.7)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (55, 124, N'Учреждение', N'Гараж', 10, 5001, 1000000, N' ', 0.5, 0.65)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (56, 94, N'Учреждение', N'Универмаг', 15, 1, 5000, N' ', 0.38, 0.08)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (57, 96, N'Учреждение', N'Универмаг', 15, 10001, 1000000, N' ', 0.31, 0.27)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (58, 97, N'Учреждение', N'Детсад', 20, 1, 5000, N' ', 0.38, 0.11)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (59, 98, N'Учреждение', N'Детсад', 20, 5001, 100000, N' ', 0.34, 0.1)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (60, 99, N'Учреждение', N'Учебное', 16, 1, 5000, N' ', 0.39, 0.09)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (61, 101, N'Учреждение', N'Учебное', 16, 10001, 1000000, N' ', 0.33, 0.07)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (62, 102, N'Учреждение', N'Больница', 20, 1, 5000, N' ', 0.4, 0.29)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (63, 103, N'Учреждение', N'Больница', 20, 5001, 10000, N' ', 0.36, 0.28)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (64, 104, N'Учреждение', N'Больница', 20, 10001, 15000, N' ', 0.32, 0.26)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (65, 105, N'Учреждение', N'Больница', 20, 15001, 1000000, N' ', 0.3, 0.25)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (66, 1, N'Жилое', N'Жилое', 18, 1, 100, N'С', 0.74, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (67, 2, N'Жилое', N'Жилое', 18, 101, 200, N'С', 0.66, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (68, 3, N'Жилое', N'Жилое', 18, 201, 300, N'С', 0.62, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (69, 4, N'Жилое', N'Жилое', 18, 301, 400, N'С', 0.6, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (70, 5, N'Жилое', N'Жилое', 18, 401, 500, N'С', 0.58, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (71, 6, N'Жилое', N'Жилое', 18, 501, 600, N'С', 0.56, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (72, 7, N'Жилое', N'Жилое', 18, 601, 700, N'С', 0.54, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (73, 8, N'Жилое', N'Жилое', 18, 701, 800, N'С', 0.53, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (74, 9, N'Жилое', N'Жилое', 18, 801, 900, N'С', 0.52, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (75, 10, N'Жилое', N'Жилое', 18, 901, 1000, N'С', 0.51, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (76, 11, N'Жилое', N'Жилое', 18, 1001, 1100, N'С', 0.5, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (77, 12, N'Жилое', N'Жилое', 18, 1101, 1200, N'С', 0.49, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (78, 13, N'Жилое', N'Жилое', 18, 1201, 1300, N'С', 0.48, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (79, 14, N'Жилое', N'Жилое', 18, 1301, 1400, N'С', 0.47, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (80, 15, N'Жилое', N'Жилое', 18, 1401, 1500, N'С', 0.47, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (81, 16, N'Жилое', N'Жилое', 18, 1501, 1700, N'С', 0.46, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (82, 17, N'Жилое', N'Жилое', 18, 1701, 2000, N'С', 0.45, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (83, 18, N'Жилое', N'Жилое', 18, 2001, 2500, N'С', 0.44, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (84, 19, N'Жилое', N'Жилое', 18, 2501, 3000, N'С', 0.43, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (85, 20, N'Жилое', N'Жилое', 18, 3001, 3500, N'С', 0.42, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (86, 21, N'Жилое', N'Жилое', 18, 3501, 4000, N'С', 0.4, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (87, 22, N'Жилое', N'Жилое', 18, 4001, 4500, N'С', 0.39, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (88, 23, N'Жилое', N'Жилое', 18, 4501, 5000, N'С', 0.38, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (89, 24, N'Жилое', N'Жилое', 18, 5001, 6000, N'С', 0.37, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (90, 25, N'Жилое', N'Жилое', 18, 6001, 7000, N'С', 0.36, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (91, 26, N'Жилое', N'Жилое', 18, 7001, 8000, N'С', 0.35, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (92, 27, N'Жилое', N'Жилое', 18, 8001, 9000, N'С', 0.34, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (93, 28, N'Жилое', N'Жилое', 18, 9001, 10000, N'С', 0.33, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (94, 29, N'Жилое', N'Жилое', 18, 10001, 11000, N'С', 0.32, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (95, 30, N'Жилое', N'Жилое', 18, 11001, 12000, N'С', 0.31, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (96, 31, N'Жилое', N'Жилое', 18, 12001, 13000, N'С', 0.3, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (97, 32, N'Жилое', N'Жилое', 18, 13001, 14000, N'С', 0.3, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (98, 33, N'Жилое', N'Жилое', 18, 14001, 15000, N'С', 0.29, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (99, 34, N'Жилое', N'Жилое', 18, 15001, 20000, N'С', 0.28, 0)
SET IDENTITY_INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] OFF


GO


SET IDENTITY_INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ON 

INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (100, 35, N'Жилое', N'Жилое', 18, 20001, 25000, N'С', 0.28, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (101, 36, N'Жилое', N'Жилое', 18, 25001, 30000, N'С', 0.28, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (102, 37, N'Жилое', N'Жилое', 18, 30001, 35000, N'С', 0.28, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (103, 38, N'Жилое', N'Жилое', 18, 35001, 40000, N'С', 0.27, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (104, 39, N'Жилое', N'Жилое', 18, 40001, 45000, N'С', 0.27, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (105, 40, N'Жилое', N'Жилое', 18, 45001, 50000, N'С', 0.26, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (106, 42, N'Жилое', N'Жилое', 18, 1, 100, N'Н', 0.92, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (107, 43, N'Жилое', N'Жилое', 18, 100, 200, N'Н', 0.82, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (108, 44, N'Жилое', N'Жилое', 18, 201, 300, N'Н', 0.78, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (109, 45, N'Жилое', N'Жилое', 18, 301, 400, N'Н', 0.74, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (110, 46, N'Жилое', N'Жилое', 18, 401, 500, N'Н', 0.71, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (111, 47, N'Жилое', N'Жилое', 18, 501, 600, N'Н', 0.69, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (112, 48, N'Жилое', N'Жилое', 18, 601, 700, N'Н', 0.68, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (113, 49, N'Жилое', N'Жилое', 18, 701, 800, N'Н', 0.67, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (114, 50, N'Жилое', N'Жилое', 18, 801, 900, N'Н', 0.66, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (115, 51, N'Жилое', N'Жилое', 18, 901, 1000, N'Н', 0.65, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (116, 52, N'Жилое', N'Жилое', 18, 1001, 1100, N'Н', 0.62, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (117, 53, N'Жилое', N'Жилое', 18, 1101, 1200, N'Н', 0.6, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (118, 54, N'Жилое', N'Жилое', 18, 1201, 1300, N'С', 0.59, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (119, 55, N'Жилое', N'Жилое', 18, 1301, 1400, N'Н', 0.58, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (120, 56, N'Жилое', N'Жилое', 18, 1401, 1500, N'Н', 0.57, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (121, 57, N'Жилое', N'Жилое', 18, 1501, 1700, N'Н', 0.55, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (122, 58, N'Жилое', N'Жилое', 18, 1701, 2000, N'Н', 0.53, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (123, 59, N'Жилое', N'Жилое', 18, 2001, 2500, N'Н', 0.52, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (124, 60, N'Жилое', N'Жилое', 18, 2501, 3000, N'Н', 0.5, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (125, 61, N'Жилое', N'Жилое', 18, 3001, 3500, N'Н', 0.48, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (126, 62, N'Жилое', N'Жилое', 18, 3501, 4000, N'Н', 0.47, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (127, 63, N'Жилое', N'Жилое', 18, 4001, 4500, N'Н', 0.46, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (128, 64, N'Жилое', N'Жилое', 18, 4501, 5000, N'Н', 0.45, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (129, 65, N'Жилое', N'Жилое', 18, 5001, 6000, N'Н', 0.43, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (130, 66, N'Жилое', N'Жилое', 18, 6001, 7000, N'Н', 0.42, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (131, 67, N'Жилое', N'Жилое', 18, 7001, 8000, N'Н', 0.41, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (132, 68, N'Жилое', N'Жилое', 18, 8001, 9000, N'Н', 0.4, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (133, 69, N'Жилое', N'Жилое', 18, 9001, 10000, N'Н', 0.39, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (134, 70, N'Жилое', N'Жилое', 18, 10001, 11000, N'Н', 0.38, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (135, 71, N'Жилое', N'Жилое', 18, 11001, 12000, N'Н', 0.38, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (136, 72, N'Жилое', N'Жилое', 18, 12001, 13000, N'Н', 0.37, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (137, 73, N'Жилое', N'Жилое', 18, 13001, 14000, N'Н', 0.37, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (138, 74, N'Жилое', N'Жилое', 18, 14001, 15000, N'Н', 0.37, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (139, 75, N'Жилое', N'Жилое', 18, 15001, 20000, N'Н', 0.37, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (140, 76, N'Жилое', N'Жилое', 18, 20001, 25000, N'Н', 0.37, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (141, 77, N'Жилое', N'Жилое', 18, 25001, 30000, N'Н', 0.36, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (142, 78, N'Жилое', N'Жилое', 18, 30001, 35000, N'Н', 0.35, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (143, 79, N'Жилое', N'Жилое', 18, 35001, 40000, N'Н', 0.35, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (144, 80, N'Жилое', N'Жилое', 18, 40001, 45000, N'Н', 0.34, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (145, 81, N'Жилое', N'Жилое', 18, 45001, 50000, N'Н', 0.34, 0)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (146, 82, N'Учреждение', N'Административное', 18, 1, 5000, NULL, 0.43, 0.09)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (147, 83, N'Учреждение', N'Административное', 18, 5001, 10000, NULL, 0.38, 0.08)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (148, 84, N'Учреждение', N'Административное', 18, 10001, 15000, NULL, 0.35, 0.07)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (149, 85, N'Учреждение', N'Административное', 18, 15001, 1000000, NULL, 0.32, 0.18)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (150, 95, N'Учреждение', N'Универмаг', 15, 5000, 10000, N' ', 0.33, 0.08)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (151, 100, N'Учреждение', N'Учебное', 16, 5001, 10000, N' ', 0.35, 0.08)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (152, 86, N'Учреждение', N'Дискоклуб', 16, 1, 5000, N' ', 0.37, 0.25)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (153, 87, N'Учреждение', N'Дискоклуб', 16, 5001, 10000, N' ', 0.33, 0.23)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (154, 88, N'Учреждение', N'Дискоклуб', 16, 10001, 1000000, N' ', 0.3, 0.2)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (155, 89, N'Учреждение', N'Театр', 15, 1, 10000, N' ', 0.29, 0.41)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (156, 90, N'Учреждение', N'Театр', 15, 11000, 15000, N' ', 0.27, 0.4)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (157, 91, N'Учреждение', N'Театр', 15, 16000, 20000, N' ', 0.22, 0.38)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (158, 92, N'Учреждение', N'Театр', 15, 21000, 30000, N' ', 0.2, 0.36)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (159, 93, N'Учреждение', N'Театр', 15, 31000, 1000000, N' ', 0.18, 0.34)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (160, 125, N'Промышленное', N'Чуголитейный цех', 16, 1, 10000, N' ', 0.3, 1.1)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (161, 126, N'Промышленное', N'Чуголитейный цех', 16, 11000, 50000, N' ', 0.25, 1)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (162, 127, N'Промышленное', N'Чуголитейный цех', 16, 51000, 100000, N' ', 0.22, 0.9)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (163, 128, N'Промышленное', N'Чуголитейный цех', 16, 101000, 150000, N' ', 0.18, 0.8)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (164, 129, N'Промышленное', N'Меднолитейный цех', 16, 5000, 10000, N' ', 0.4, 2.5)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (165, 130, N'Промышленное', N'Меднолитейный цех', 16, 11000, 20000, N' ', 0.35, 2)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (166, 131, N'Промышленное', N'Меднолитейный цех', 16, 21000, 30000, N' ', 0.25, 1.5)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (167, 132, N'Промышленное', N'Меднолитейный цех', 16, 31000, 1000000, N' ', 0.2, 1.2)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (168, 133, N'Промышленное', N'Термический цех', 16, 1, 10000, N' ', 0.4, 1.3)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (169, 134, N'Промышленное', N'Термический цех', 16, 10001, 30000, N' ', 0.3, 1.2)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (170, 135, N'Промышленное', N'Термический цех', 16, 30001, 75000, N' ', 0.25, 1)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (171, 136, N'Промышленное', N'Кузнечный цех', 16, 1, 10000, N' ', 0.4, 0.7)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (172, 137, N'Промышленное', N'Кузнечный цех', 16, 10001, 50000, N' ', 0.3, 0.6)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (173, 138, N'Промышленное', N'Кузнечный цех', 16, 50001, 100000, N' ', 0.25, 0.5)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (174, 139, N'Промышленное', N'Кузнечный цех', 16, 100001, 1000000, N' ', 0.15, 0.3)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (175, 140, N'Промышленное', N'Инструментальный цех', 16, 1, 10000, N' ', 0.55, 0.4)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (176, 141, N'Промышленное', N'Инструментальный цех', 16, 10001, 50000, N' ', 0.45, 0.25)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (177, 142, N'Промышленное', N'Инструментальный цех', 16, 50001, 100000, N' ', 0.4, 0.15)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (178, 143, N'Промышленное', N'Инструментальный цех', 16, 100001, 200000, N' ', 0.38, 0.12)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (179, 144, N'Промышленное', N'Инструментальный цех', 16, 200001, 1000000, N' ', 0.35, 0.08)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (180, 145, N'Промышленное', N'Деревообделочный цех', 16, 1, 5000, N' ', 0.6, 0.6)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (181, 146, N'Промышленное', N'Деревообделочный цех', 16, 5001, 10000, N' ', 0.55, 0.5)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (182, 147, N'Промышленное', N'Деревообделочный цех', 16, 10001, 50000, N' ', 0.45, 0.45)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (183, 148, N'Промышленное', N'Деревообделочный цех', 16, 50001, 100000, N' ', 0.4, 0.4)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (184, 149, N'Промышленное', N'Цех металл.конструкций', 16, 1, 100000, N' ', 0.38, 0.53)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (185, 150, N'Промышленное', N'Цех металл.конструкций', 16, 100001, 150000, N' ', 0.35, 0.45)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (186, 151, N'Промышленное', N'Цех металл.конструкций', 16, 105001, 1000000, N' ', 0.3, 0.35)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (187, 152, N'Промышленное', N'Цех покрытий', 16, 1, 2000, N' ', 0.65, 5)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (188, 153, N'Промышленное', N'Цех покрытий', 16, 2001, 5000, N' ', 0.6, 4)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (189, 154, N'Промышленное', N'Цех покрытий', 16, 5001, 10000, N' ', 0.55, 3)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (190, 155, N'Промышленное', N'Цех покрытий', 16, 10001, 1000000, N' ', 0.45, 0.45)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (191, 156, N'Промышленное', N'Ремонтный цех', 16, 1, 10000, N' ', 0.6, 0.2)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (192, 157, N'Промышленное', N'Ремонтный цех', 16, 10001, 20000, N' ', 0.5, 0.15)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (193, 158, N'Промышленное', N'Ремонтный цех', 16, 20001, 1000000, N' ', 0.45, 0.1)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (194, 159, N'Промышленное', N'Паровозное депо', 16, 1, 5000, N' ', 0.7, 0.4)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (195, 160, N'Промышленное', N'Паровозное депо', 16, 50001, 10000, N' ', 0.65, 0.3)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (196, 161, N'Промышленное', N'Паровозное депо', 16, 10001, 1000000, N' ', 0.6, 0.25)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (197, 162, N'Промышленное', N'Котельный цех', 16, 1, 1000000, N' ', 0.25, 0.6)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (198, 163, N'Промышленное', N'Котельная', 16, 1, 2000, N' ', 0.1, 0.5)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (199, 200, N'Промышленное', N'Вспомогательное', 16, 10001, 1000000, N' ', 0.25, 0.1)

SET IDENTITY_INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] OFF


GO


SET IDENTITY_INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ON 

INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (200, 201, N'Промышленное', N'Проходная', 16, 1, 500, N' ', 1.3, 0.1)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (201, 202, N'Промышленное', N'Проходная', 16, 501, 2000, N' ', 1.2, 0.1)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (202, 203, N'Промышленное', N'Проходная', 16, 2001, 5000, N' ', 0.7, 0.15)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (203, 204, N'Промышленное', N'Проходная', 16, 5001, 1000000, N' ', 0.55, 0.1)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (204, 205, N'Промышленное', N'Охрана', 16, 1, 5000, N' ', 0.38, 0.1)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (205, 206, N'Промышленное', N'Охрана', 16, 5001, 10000, N' ', 0.33, 0.1)
INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] ([id], [nomer], [sategory], [name_zd], [tr_tv_co], [vmin], [vmax], [pr_year], [q_ot], [q_vt]) VALUES (206, 207, N'Промышленное', N'Охрана', 16, 10001, 1000000, N' ', 0.31, 0.1)
SET IDENTITY_INSERT [dbo].[10_teplovye_harakteristiki_zdaniy] OFF

GO

SET IDENTITY_INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ON 

INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (1, N'1', N'Листы из алюминия', N'0,3; 0,5-1', N'Металлизационное', N'ГОСТ 21631-76', N'АДО, АД1, АМц, Амг2, В95', N'Негорючие')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (2, N'2', N'Ленты из алюминия', N'0,25-1', N'Металлизационное', N'ГОСТ 13726-78', N'АДО, АД1, АМц, Амг2, В95', N'Негорючие')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (3, N'3', N'Сталь оцинкованная', N'0,35-1', N'Металлизационное', N'ГОСТ 14918-80', NULL, N'Негорючие')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (4, N'4', N'Сталь кровельная', N'0,5-0,8', N'Металлизационное', N'ГОСТ 14918-80', NULL, N'Негорючие')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (5, N'5', N'Прокат углеродистой стали', N'0,35-1', N'Металлизационное', N'ГОСТ 16523-70', NULL, N'Негорючие')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (6, N'6', N'Оболочки гофрированные', N'0,2; 2,5', N'Металлизационное', N'ОСТ 36-67-82', NULL, N'Негорючие; Горючие')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (7, N'7', N'Металлопласт', N'0,8-1,3', N'Металлизационное', N'ТУ 14-1-1114-74', NULL, N'Трудногорючие')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (8, N'8', N'Стеклотекстолит конструкционный', N'0,5-1,2', N'Синтетический полимер', N'ГОСТ 10292-74Е', NULL, N'Горючие')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (9, N'9', N'Армопластмассовый', N'2,2', N'Синтетический полимер', NULL, N'АПМ-1', N'Горючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (10, N'10', N'Армопластмассовый', N'2,1', N'Синтетический полимер', NULL, N'АПМ-2', N'Трудногорючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (11, N'11', N'Армопластмассовый', N'2,1', N'Синтетический полимер', NULL, N'АПМ-3', N'Горючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (12, N'12', N'Стеклопластик рулонный РСТ', N'0,25-0,5', N'Синтетический полимер', N'ТУ 6-11-145-80', N'РСТ-А, РСТ-Б, РСТ-Х', N'Трудногорючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (13, N'13', N'Стеклопластик фенольный', N'0,3; 0,6', N'Синтетический полимер', N'ТУ 6-11-150-76', NULL, N'Горючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (14, N'14', N'Пленка винипластовая КПО', N'0,4-1', N'Синтетический полимер', N'ГОСТ 16398-81', N'КПО', N'Горючая')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (15, N'15', N'Пленка из поливинилхлоридного сырья', N'1,3', N'Синтетический полимер', N'ТУ 63.032.3-88', NULL, N'Горючая')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (16, N'16', N'Стеклотекстолит  листовой', N'0,3', N'Синтетический полимер', N'ТУ 36-1583-88', N'СТПЛ-СБ', N'Трудногорючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (17, N'17', N'Стеклотекстолит  листовой', N'0,5', N'Синтетический полимер', N'ТУ 36-1583-88', N'СТПЛ-ТБ', N'Трудногорючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (18, N'18', N'Стеклотекстолит  листовой', N'0,8', N'Синтетический полимер', N'ТУ 36-1583-88', N'СТПЛ-ВП', N'Трудногорючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (19, N'РУБ', N'Рубероид', N'2-3', N'Природный полимер', N'ГОСТ 10923-82', N'РКК-420', N'Горючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (20, N'20', N'Стеклорубероид', N'2,5', N'Природный полимер', N'ГОСТ 15879-70', NULL, N'Горючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (21, N'21', N'Толь', N'1,0-1,5', N'Природный полимер', N'ГОСТ 10999-76', N'ТКК-350, ТКК-400', N'Горючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (22, N'22', N'Пергамин', N'1,0-1,5', N'Природный полимер', N'ГОСТ 2697-83', NULL, N'Горючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (23, N'23', N'Рубероид, покрытый стеклотканью', NULL, N'Природный полимер', N'ТУ 21 ЭССР 48-83', NULL, N'Горючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (24, N'24', N'Изол', N'2', N'Природный полимер', N'ГОСТ 10296-79', NULL, N'Горючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (25, N'25', N'Стеклоцемент текстолитовый', N'1,5-2', N'Минеральные', N'ТУ 36-940-85', NULL, N'Негорючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (26, N'26', N'Листы асбестоцементные плоские', N'6-10', N'Минеральные', N'ГОСТ 18124-75', NULL, N'Негорючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (27, N'27', N'Листы асбестоцементные волнистые', N'5-8', N'Минеральные', N'ГОСТ 16233-77', NULL, N'Негорючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (28, N'28', N'Штукатурка асбестоцементная', N'10-20', N'Минеральные', NULL, NULL, N'Негорючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (29, N'29', N'Фольга алюминиевая', N'0,5-1,5', N'Дублированные фольгой', N'ТУ 36-1177 -77', NULL, N'Горючий; Трудногорючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (30, N'30', N'Фольгорубероид', N'1,7-2', N'Дублированные фольгой', N'ТУ 21 ЭССР 69-83', NULL, N'Горючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (31, N'31', N'Фольгоизол', N'2-2,5', N'Дублированные фольгой', N'ГОСТ 20429-84', NULL, N'Горючий')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (32, N'СЛ', N'Сталь листовая', NULL, N'Металлизационное', NULL, NULL, N'Негорючие')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (33, N'ОЦ', N'Сталь оцинкованная', NULL, N'Металлизационное', NULL, NULL, N'Негорючие')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (34, N'СЛХ', N'Сталь листовая холоднокатанная', NULL, N'Металлизационное', NULL, NULL, N'Негорючие')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (35, N'СТК', N'Cтеклоткань', NULL, N'Синтетический полимер', NULL, NULL, N'Негорючие')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (36, N'АСБ', N'Асбест (хризалит)', NULL, N'Минеральные', NULL, NULL, N'Негорючие')
INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] ([id], [kod], [znachenie], [tolschina__mm], [primechanie], [gost], [marki], [goryuchest]) VALUES (38, N'ПЭ', N'Полиэтилен', NULL, N'Синтетический полимер', NULL, NULL, N'Горючие')
SET IDENTITY_INSERT [dbo].[11_material_pokrovnogo_sloya_izolyatsii] OFF

GO


SET IDENTITY_INSERT [dbo].[12_materialy_konstruktsiy_kamer] ON 

INSERT [dbo].[12_materialy_konstruktsiy_kamer] ([id], [mat_construct]) VALUES (1, N'Железобетон')
INSERT [dbo].[12_materialy_konstruktsiy_kamer] ([id], [mat_construct]) VALUES (2, N'Сталь')
SET IDENTITY_INSERT [dbo].[12_materialy_konstruktsiy_kamer] OFF
GO

SET IDENTITY_INSERT [dbo].[13_klimaticheskie_dannye] ON 

INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (1, N'Актобе', 203, -31, -21, -7.3, -15.6, -14.9, -14.4, -7.7, 5.4, 14.7, 20, 22.3, 20.3, 13.5, 4.6, -4.6, -11.6, 4)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (2, N'Алматы', 166, -25, -10, -2.1, -7.1, -6.8, -5.1, 1.9, 10.7, 16.2, 20.9, 23.1, 22.3, 17, 9.6, 1.1, -4.4, 8.9)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (3, N'Амангельды', 0, 0, 0, 0, 0, -16.8, -16.3, -9.3, 5.7, 14.9, 20.7, 22.9, 20.4, 13.5, 4.4, -5.6, -13.2, 3.4)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (4, N'Аральск', 0, 0, 0, 0, 0, -12.8, -11.8, -3.9, 9.1, 17.7, 24, 26.4, 24.3, 17.3, 7.9, -1.8, -8.8, 7.3)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (5, N'Архангельск', 251, -32, -19, -4.7, -12.5, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (6, N'Астана', 215, -35, -22, -8.7, -17.4, -16.7, -16.2, -10.4, 2.9, 12.7, 17.9, 20.4, 17.9, 11.4, 2.9, -7.2, -14, 1.8)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (7, N'Астрахань', 172, -22, -8, -1.6, -6.8, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (8, N'Атбасар', 0, 0, 0, 0, 0, -18.1, -17.5, -11.5, 2.1, 12.4, 17.9, 20.1, 17.8, 11.4, 2.5, -7.9, -15.2, 1.2)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (9, N'Атырау', 0, 0, 0, 0, 0, -9.6, -8.8, -1.8, 9.4, 17.9, 23.3, 25.6, 23.7, 16.7, 8.3, -0.1, -5.7, 8.9)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (10, N'Баканас', 0, 0, 0, 0, 0, -11.8, -9.9, -0.1, 11, 17.7, 23.3, 25.6, 23.4, 16.7, 8.3, -1.1, -8.2, 8)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (11, N'Баку', 119, -4, 1, 5.1, 3.8, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (12, N'Балхаш', 190, -32, -20, -6.9, -15.2, -14.7, -13.4, -5.2, 7.5, 15.8, 21.9, 24, 21.8, 15.3, 6.6, -3.7, -11.3, 5.4)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (13, N'Барнаул', 219, -39, -23, -8.3, -17.7, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (14, N'Бахты', 0, 0, 0, 0, 0, -13.6, -11.2, -2.1, 9.3, 15.7, 20.6, 23, 21.6, 15.3, 7.2, -2.8, -10.6, 6)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (15, N'Бейнеу', 0, 0, 0, 0, 0, -7.5, -7.1, -0.2, 11.6, 19.1, 25, 27.7, 25.9, 18.5, 9.4, 1.1, -4.4, 9.9)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (16, N'Брянск', 206, -24, -13, -2.6, -8.5, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (17, N'Вильнюс', 194, -23, -9, -0.9, -5.5, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (18, N'Владивосток', 201, -25, -16, -4.8, -14.4, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (19, N'Волгоград', 182, -22, -13, -3.4, -9.2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (20, N'Воронеж', 199, -25, -14, -3.4, -9.3, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (21, N'Ганюшкино', 0, 0, 0, 0, 0, -7.4, -6.6, -0.9, 9.7, 17.8, 22.9, 25, 23.2, 16.5, 8.8, 1.2, -4.1, 8.8)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (22, N'Екатеринбург', 228, -31, -20, -6.4, -15.3, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (23, N'Енисейск', 245, -47, -28, -9.8, -22, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (24, N'Ертис', 0, 0, 0, 0, 0, -18, -17.4, -10.7, 3.1, 12.6, 18.6, 20.6, 17.6, 11.6, 3, -8.3, -15.1, 1.5)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (25, N'Жалпактал', 0, 0, 0, 0, 0, -11.8, -11.6, -4.7, 8, 16.3, 21.6, 24.1, 22.1, 15.1, 6.3, -1.9, -8.1, 6.3)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (26, N'Жаркент', 0, 0, 0, 0, 0, -9, -5.9, 3.2, 12.6, 17.8, 22.1, 24, 22.8, 17.2, 10, 1, -5.8, 9.2)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (27, N'Жезказган', 0, 0, 0, 0, 0, -15.1, -14.2, 6.6, 7.3, 15.7, 21.9, 24.1, 21.4, 14.5, 5.2, -4.5, -11.8, 4.8)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (28, N'Жымпиты', 0, 0, 0, 0, 0, -13.7, -13.2, -6.7, 6.7, 15.6, 21.1, 23.7, 21.7, 14.5, 5.5, -2.8, -9.6, 5.7)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (29, N'Зайсан', 0, 0, 0, 0, 0, -17, -14.6, -6.8, 6.9, 15, 20.8, 23, 21.3, 15.2, 6.4, -5.8, -14.4, 4.2)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (30, N'Златоуст', 232, -30, -20, -6.6, -15.4, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (31, N'Иваново', 217, -28, -16, -4.4, -11.8, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (32, N'Иркутск', 241, -38, -25, -8.9, -20.9, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (33, N'Казалы', 0, 0, 0, 0, 0, -11.1, -9.6, -1.7, 10.5, 18.8, 24.1, 26.2, 23.9, 17, 8.3, -0.8, -7.5, 8.2)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (34, N'Казань', 218, -30, -18, -5.7, -13.5, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (35, N'Караганды', 212, -32, -20, -7.5, -15.1, -14.5, -14.1, -8.2, 4.1, 12.7, 18.3, 20.4, 18, 11.9, 3, -6.6, -12.8, 2.7)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (36, N'Караулкельды', 0, 0, 0, 0, 0, -12.7, -12.3, -5.6, 8.2, 16.3, 22, 23.8, 22.4, 15.3, 5.8, -2.5, -9.5, 6.2)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (37, N'Каркаралы', 0, 0, 0, 0, 0, -14, -13.7, -7.6, 3, 10.7, 15.9, 18, 15.7, 10, 2.2, -6.8, -12.3, 1.8)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (38, N'Карсакпай', 0, 0, 0, 0, 0, -15.1, -14.5, -7.7, -5.7, 14.7, 20.7, 23, 20.5, 13.6, 4.4, -5.2, -12.2, 4)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (39, N'Киев', 187, -21, -10, -1.1, -5.9, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (40, N'Киров', 231, -31, -19, -5.8, -14.2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (41, N'Кишинев', 166, -15, -7, 0.6, -3.5, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (42, N'Кокпекты', 0, 0, 0, 0, 0, -20.6, -18.3, -10.4, 3.9, 12.7, 18.3, 20.8, 18.7, 12.3, 3.2, -8.7, -17.5, 1.2)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (43, N'Кокшетау', 0, 0, 0, 0, 0, -15.8, -15.1, -9.4, 3, 12.2, 17.6, 19.7, 17.3, 11.4, 3, -6.9, -13.5, 2)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (44, N'Костанай', 213, -35, -22, -8.7, -17.7, -17.2, -16.7, -10.2, 3.2, 12.8, 18.5, 20.3, 18.1, 11.9, 3.1, -6.6, -14.1, 1.9)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (45, N'Красноярск', 235, -40, -22, -7.2, -17.1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (46, N'Курск', 198, -24, -14, -3, -8.6, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (47, N'Кызылжар', 0, 0, 0, 0, 0, -15.5, -15.5, -7.7, 7.1, 14.9, 21, 23.4, 20.6, 13.8, 4.7, -5.1, -12.5, 4.1)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (48, N'Кызылорда', 166, -25, -10, -2.1, -7.1, -9.2, -7.1, 0.8, 12, 19.4, 24.5, 26, 24, 17.2, 8.9, 0.1, -6.3, 9.2)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (49, N'Луганск', 180, -25, -10, -1.6, -6.6, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (50, N'Львов', 183, -19, -7, 0.3, -3.9, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (51, N'Магнитогорск', 218, -34, -22, -7.9, -16.9, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (52, N'Махачкала', 151, -14, -2, 2.6, -0.4, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (53, N'Минск', 203, -25, -10, -1.2, -6.9, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (54, N'Минусинск', 226, -42, -27, -9.5, -21.2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (55, N'Мичуринск', 202, -26, -15, -4.3, -10.8, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (56, N'Москва', 205, -25, -14, -3.2, -9.4, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (57, N'Мурманск', 281, -28, -18, -3.3, -10.1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (58, N'Нижний Новгород', 218, -30, -16, -4.7, -12, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (59, N'Нижний Тагил', 238, -34, -21, -6.6, -16.1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (60, N'Новороссийск', 134, -13, -2, 4.4, 2.6, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (61, N'Новосибирск', 227, -39, -24, -9.1, -19, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (62, N'Одесса', 165, -17, -6, 1, -2.5, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (63, N'Омск', 220, -37, -23, -7.7, -19.2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (64, N'Оренбург', 201, -29, -20, -8.1, -14.8, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (65, N'Орск', 204, -29, -21, -7.9, -16.4, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (66, N'Павлодар', 0, 0, 0, 0, 0, -17.5, -16.8, -10, 3.9, 13.1, 19.2, 21.4, 18.6, 12.3, 3.5, -7.4, -14.5, 2.2)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (67, N'Пенза', 206, -27, -17, -5.1, -12.1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (68, N'Пермь', 226, -34, -20, -6.4, -15.1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (69, N'Петропавловск', 0, 0, 0, 0, 0, -18.3, -16.9, -10.2, 2, 11.5, 16.9, 18.7, 16.3, 10.6, 2, -8.2, -15.3, 0.8)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (70, N'Рига', 205, -20, -9, -0.6, -5, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (71, N'Ростов-на-Дону', 175, -22, -8, -1.1, -5.7, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (72, N'Рязань', 212, -27, -16, -4.2, -11.1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (73, N'Самара', 206, -27, -18, -6.1, -13.8, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (74, N'Самарканд', 132, -13, 3, 2.8, -0.3, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (75, N'Санкт-Петербург', 219, -25, -11, -2.2, -7.9, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (76, N'Саратов', 198, -25, -16, -5, -11.9, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (77, N'Семей', 202, -38, -21, -8, -16.2, -16, -15.4, -8.4, 4.9, 14.2, 19.9, 21.9, 19.3, 12.3, 4.6, -5.9, -12.1, 3.2)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (78, N'Смоленск', 210, -26, -13, -2.7, -8.6, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (79, N'Стерлитамак', 210, -36, -20, -7.1, -15.2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (80, N'Тайпак', 0, 0, 0, 0, 0, -11.9, -11.2, -2.1, 9.3, 17.4, 22.7, 25.1, 23.1, 16.1, 6.6, -1.6, -7.8, 7.2)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (81, N'Талдыкорган', 0, 0, 0, 0, 0, -10.2, -8.1, -0.2, 10.1, 16.2, 21.1, 23.4, 21.7, 15.9, 8.2, -1, -7.4, 7.5)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (82, N'Таллинн', 221, -21, -9, -0.8, -5.5, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (83, N'Тараз', 0, 0, 0, 0, 0, -5.2, -3.3, 3.2, 11.2, 16.7, 21.8, 24.3, 22.3, 16.5, 9.4, 1.8, -3.2, 9.6)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (84, N'Ташкент', 130, -15, -6, 2.4, -0.9, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (85, N'Тбилиси', 152, -7, 0, 4.2, 0.9, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (86, N'Тверь', 219, -29, -15, -3.7, -10.4, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (87, N'Тобольск', 220, -36, -22, -7, -18.5, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (88, N'Томск', 234, -40, -25, -8.8, -19.2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (89, N'Торгай', 0, 0, 0, 0, 0, -16.6, -15.8, -8.3, 6.1, 15.9, 22, 24.3, 21.9, 15, 5.4, -4.6, -12.8, 4.4)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (90, N'Тула', 207, -28, -14, -3.8, -10.1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (91, N'Туркестан', 0, 0, 0, 0, 0, -5.4, -2.2, 5.3, 14.1, 20.6, 25.9, 28.3, 26.4, 19.8, 11.2, 2.9, -2.6, 12)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (92, N'Тюмень', 220, -35, -21, -5.7, -16.6, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (93, N'Уил', 0, 0, 0, 0, 0, -12.8, -12, -5.1, 8, 16.5, 22.2, 24.5, 22.7, 15.5, 6.6, -2.5, -9, 6.2)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (94, N'Уланбель', 0, 0, 0, 0, 0, -9.6, -7.6, 0.6, 11.9, 18.9, 24.3, 26.4, 24.1, 17.3, 8.2, -0.4, -6.4, 9.2)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (95, N'Улан-Удэ', 235, -38, -28, -10.6, -25.4, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (96, N'Ульяновск', 213, -31, -18, -5.7, -13.8, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (97, N'Уральск', 199, -30, -18, -6.5, -14.2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (98, N'Усть-Каменогорк', 0, 0, 0, 0, 0, -16.2, -15.5, -8, 4.7, 13.7, 18.8, 20.7, 18.3, 12.3, 5, -6.2, -13.5, 2.8)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (99, N'Уфа', 211, -29, -19, -6.4, -14.1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)

SET IDENTITY_INSERT [dbo].[13_klimaticheskie_dannye] OFF

GO

SET IDENTITY_INSERT [dbo].[13_klimaticheskie_dannye] ON 

INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (100, N'Ушарал', 0, 0, 0, 0, 0, -13.2, -11.5, -2.3, 9.5, 16.5, 21.5, 24.3, 22.1, 16.2, 8.2, -1.6, -8.9, 6.7)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (101, N'Форт Шевченко', 0, 0, 0, 0, 0, -3, -2.4, 2.4, 10.2, 17.6, 22.8, 25.6, 24.6, 19.4, 12.3, 5.3, 0, 11.2)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (102, N'Хабаровск', 205, -32, -23, -10.1, -22.1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (103, N'Харьков', 189, -23, -11, -2.1, -7.3, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (104, N'Челябинск', 216, -29, -20, -7.1, -15.5, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (105, N'Чита', 240, -38, -30, -11.6, -26.6, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (106, N'Шалкар', 0, 0, 0, 0, 0, -14.6, -14.4, -6.3, 7.9, 16.8, 22.7, 25, 23, 15.6, 6.3, -3, -10.6, 5.7)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (107, N'Шымкент', 0, 0, 0, 0, 0, -2.1, -0.1, 5.5, 13, 18.5, 23.4, 26.2, 24.7, 19.2, 12.2, 5.1, 0.3, 12.2)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (108, N'Ыргыз', 0, 0, 0, 0, 0, -15.8, -14.6, -7.3, 7.5, 17, 22.7, 25, 22.7, 15.7, 6.7, -3.6, -11.4, 5.4)
INSERT [dbo].[13_klimaticheskie_dannye] ([id], [sity], [tag], [otopl_pr], [vetn_pr], [t_sr], [t_sr_min], [t_1month], [t_2month], [t_3month], [t_4month], [t_5month], [t_6month], [t_7month], [t_8month], [t_9month], [t_10month], [t_11month], [t_12month], [t_year]) VALUES (109, N'Эмба', 0, 0, 0, 0, 0, -14.5, -13.7, -7, 6.7, 15.8, 21.6, 23.9, 21.8, 14.5, 5.3, -3.6, -10.8, 5)
SET IDENTITY_INSERT [dbo].[13_klimaticheskie_dannye] OFF
GO

SET IDENTITY_INSERT [dbo].[14_sherohovatost_stalnyh_trub] ON 

INSERT [dbo].[14_sherohovatost_stalnyh_trub] ([id], [tip], [s_min], [s_max]) VALUES (1, N'Бесшовные новые и чистые', 0.01, 0.02)
INSERT [dbo].[14_sherohovatost_stalnyh_trub] ([id], [tip], [s_min], [s_max]) VALUES (2, N'Сварные после нескольких лет эксплуатации', 0.15, 0.3)
INSERT [dbo].[14_sherohovatost_stalnyh_trub] ([id], [tip], [s_min], [s_max]) VALUES (3, N'Сварные новые и чистые', 0.03, 0.12)
INSERT [dbo].[14_sherohovatost_stalnyh_trub] ([id], [tip], [s_min], [s_max]) VALUES (4, N'Сварные с малой коррозией после очистки', 0.1, 0.2)
INSERT [dbo].[14_sherohovatost_stalnyh_trub] ([id], [tip], [s_min], [s_max]) VALUES (5, N'Сварные умеренно заржавевшие', 0.3, 0.7)
INSERT [dbo].[14_sherohovatost_stalnyh_trub] ([id], [tip], [s_min], [s_max]) VALUES (6, N'Сварные старые заржавевшие', 0.3, 0.7)
INSERT [dbo].[14_sherohovatost_stalnyh_trub] ([id], [tip], [s_min], [s_max]) VALUES (7, N'Сварные сильно заржавевшие', 2, 4)
INSERT [dbo].[14_sherohovatost_stalnyh_trub] ([id], [tip], [s_min], [s_max]) VALUES (8, N'Сварные с большими отложениями', 2, 4)
INSERT [dbo].[14_sherohovatost_stalnyh_trub] ([id], [tip], [s_min], [s_max]) VALUES (9, N'Оцинкованные новые и чистые', 0.1, 0.2)
INSERT [dbo].[14_sherohovatost_stalnyh_trub] ([id], [tip], [s_min], [s_max]) VALUES (10, N'Оцинкованные после нескольких лет эксплуатации', 0.4, 0.7)
SET IDENTITY_INSERT [dbo].[14_sherohovatost_stalnyh_trub] OFF

GO

SET IDENTITY_INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ON 

INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (1, 15, 60, 40)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (2, 25, 80, 60)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (3, 40, 80, 60)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (4, 50, 100, 80)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (5, 65, 140, 80)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (6, 80, 160, 80)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (7, 100, 160, 80)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (8, 125, 160, 80)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (9, 150, 160, 100)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (10, 200, 180, 100)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (11, 250, 180, 100)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (12, 300, 200, 100)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (13, 350, 200, 100)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (14, 400, 220, 120)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (15, 450, 220, 120)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (16, 500, 220, 120)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (17, 600, 240, 120)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (18, 700, 240, 120)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (19, 800, 240, 120)
INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] ([id], [d], [s_ton], [s_kan]) VALUES (20, 900, 260, 120)
SET IDENTITY_INSERT [dbo].[17_predelnaya_tolschina_teploizolyatsii_pri_podzemke] OFF

GO

SET IDENTITY_INSERT [dbo].[18_fizicheskie_svoystva_vody] ON 

INSERT [dbo].[18_fizicheskie_svoystva_vody] ([id], [t], [p], [g]) VALUES (1, 0, 0.9999, 1795000)
INSERT [dbo].[18_fizicheskie_svoystva_vody] ([id], [t], [p], [g]) VALUES (2, 10, 0.9997, 1310000)
INSERT [dbo].[18_fizicheskie_svoystva_vody] ([id], [t], [p], [g]) VALUES (3, 20, 0.9982, 1010000)
INSERT [dbo].[18_fizicheskie_svoystva_vody] ([id], [t], [p], [g]) VALUES (4, 30, 0.9957, 804000)
INSERT [dbo].[18_fizicheskie_svoystva_vody] ([id], [t], [p], [g]) VALUES (5, 40, 0.9922, 659000)
INSERT [dbo].[18_fizicheskie_svoystva_vody] ([id], [t], [p], [g]) VALUES (6, 50, 0.9881, 556000)
INSERT [dbo].[18_fizicheskie_svoystva_vody] ([id], [t], [p], [g]) VALUES (7, 60, 0.9832, 478000)
INSERT [dbo].[18_fizicheskie_svoystva_vody] ([id], [t], [p], [g]) VALUES (8, 70, 0.9778, 416000)
INSERT [dbo].[18_fizicheskie_svoystva_vody] ([id], [t], [p], [g]) VALUES (9, 80, 0.9718, 367000)
INSERT [dbo].[18_fizicheskie_svoystva_vody] ([id], [t], [p], [g]) VALUES (10, 90, 0.9653, 328000)
INSERT [dbo].[18_fizicheskie_svoystva_vody] ([id], [t], [p], [g]) VALUES (11, 100, 0.9584, 296000)
INSERT [dbo].[18_fizicheskie_svoystva_vody] ([id], [t], [p], [g]) VALUES (12, 120, 0.9434, 246000)
INSERT [dbo].[18_fizicheskie_svoystva_vody] ([id], [t], [p], [g]) VALUES (13, 140, 0.9264, 212000)
INSERT [dbo].[18_fizicheskie_svoystva_vody] ([id], [t], [p], [g]) VALUES (14, 160, 0.9075, 192000)
INSERT [dbo].[18_fizicheskie_svoystva_vody] ([id], [t], [p], [g]) VALUES (15, 180, 0.887, 174000)
INSERT [dbo].[18_fizicheskie_svoystva_vody] ([id], [t], [p], [g]) VALUES (16, 200, 0.865, 162000)
SET IDENTITY_INSERT [dbo].[18_fizicheskie_svoystva_vody] OFF

GO

SET IDENTITY_INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ON 

INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (1, 0, N'15', N'20', N'25', N'32', N'100', N'70', N'125', N'150', N'184', N'207', N'259', N'300', N'359', N'408', N'414', N'466', N'514', N'612', N'700', N'800', N'898', N'996', N'1096', N'1192', N'1392')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (2, 1, N'0,013026
0,021', N'0,02867
0,026', N'0,051689
0,03', N'0,098802
0,035', N'2,0676
0,075', N'0,79698
0,059', N'3,7044
0,086', N'6,0166
0,097', N'10,36
0,111', N'14,175
0,12', N'25,52
0,138', N'37,712
0,152', N'60,755
0,171', N'85,355
0,186', N'88,357
0,187', N'120,93
0,202', N'156,59
0,215', N'248,84
0,241', N'353,91
0,262', N'502,83
0,285', N'682,48
0,307', N'896,99
0,328', N'1155,7
0,349', N'1441,4
0,368', N'2168,7
0,406')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (3, 1.2, N'0,014266
0,023', N'0,030876
0,028', N'0,056858
0,033', N'0,11009
0,039', N'2,2605
0,082', N'0,87802
0,065', N'4,092
0,095', N'6,6369
0,107', N'11,387
0,122', N'15,592
0,132', N'28,109
0,152', N'41,434
0,167', N'66,795
0,188', N'93,615
0,204', N'97,334
0,206', N'132,9
0,222', N'171,88
0,236', N'272,59
0,264', N'389,03
0,288', N'552,23
0,313', N'749,17
0,337', N'984,51
0,36', N'1265
0,382', N'1578,5
0,403', N'2377
0,445')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (4, 1.4, N'0,015507
0,025', N'0,034184
0,031', N'0,062027
0,036', N'0,11856
0,042', N'2,4535
0,089', N'0,94556
0,07', N'4,4366
0,103', N'7,1951
0,116', N'12,32
0,132', N'16,892
0,143', N'30,513
0,165', N'44,907
0,181', N'72,124
0,203', N'100,96
0,22', N'105,37
0,223', N'143,67
0,24', N'186,45
0,256', N'295,3
0,286', N'420,1
0,311', N'598,1
0,339', N'811,41
0,365', N'1063,8
0,389', N'1370,9
0,414', N'1707,8
0,436', N'2569,3
0,481')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (5, 1.6, N'0,016747
0,027', N'0,036389
0,033', N'0,065473
0,038', N'0,12703
0,045', N'2,6189
0,095', N'1,0131
0,075', N'4,7382
0,11', N'7,6913
0,124', N'13,253
0,142', N'18,073
0,153', N'32,547
0,176', N'48,133
0,194', N'77,454
0,218', N'108,3
0,236', N'112,45
0,238', N'153,85
0,257', N'199,56
0,274', N'315,95
0,306', N'449,82
0,333', N'638,68
0,362', N'866,99
0,39', N'1140,4
0,417', N'1467
0,443', N'1829,2
0,467', N'2751
0,515')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (6, 1.8, N'0,017988
0,029', N'0,038594
0,035', N'0,070642
0,041', N'0,1355
0,048', N'2,7843
0,101', N'1,0806
0,08', N'5,0397
0,117', N'8,1875
0,132', N'14
0,15', N'19,136
0,162', N'34,581
0,187', N'51,11
0,206', N'82,073
0,231', N'115,18
0,251', N'119,54
0,253', N'163,43
0,273', N'211,94
0,291', N'335,57
0,325', N'478,19
0,354', N'679,26
0,385', N'920,34
0,414', N'1208,8
0,442', N'1556,4
0,47', N'1938,9
0,495', N'2916,5
0,546')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (7, 2, N'0,019228
0,031', N'0,0408
0,037', N'0,074088
0,043', N'0,14397
0,051', N'2,9497
0,107', N'1,1482
0,085', N'5,3412
0,124', N'8,6217
0,139', N'14,84
0,159', N'20,199
0,171', N'36,615
0,198', N'53,839
0,217', N'86,691
0,244', N'121,61
0,265', N'126,16
0,267', N'172,41
0,288', N'223,59
0,307', N'353,12
0,342', N'503,85
0,373', N'716,31
0,406', N'971,47
0,437', N'1274,4
0,466', N'1639,2
0,495', N'2048,6
0,523', N'3076,8
0,576')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (8, 2.2, N'0,019849
0,032', N'0,043005
0,039', N'0,077534
0,045', N'0,14961
0,053', N'3,0876
0,112', N'1,2022
0,089', N'5,5996
0,13', N'9,0559
0,146', N'15,587
0,167', N'21,262
0,18', N'38,464
0,208', N'56,568
0,228', N'90,955
0,256', N'127,57
0,278', N'132,3
0,28', N'180,79
0,302', N'234,52
0,322', N'370,68
0,359', N'528,17
0,391', N'751,6
0,426', N'1018,2
0,458', N'1337,3
0,489', N'1722
0,52', N'2146,5
0,548', N'3231,7
0,605')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (9, 2.4, N'0,021089
0,034', N'0,045211
0,041', N'0,080979
0,047', N'0,15808
0,056', N'3,253
0,118', N'1,2562
0,093', N'5,8581
0,136', N'9,4901
0,153', N'16,24
0,174', N'22,207
0,188', N'40,129
0,217', N'59,298
0,239', N'95,218
0,268', N'133,08
0,29', N'138,44
0,293', N'189,17
0,316', N'244,72
0,336', N'388,23
0,376', N'552,48
0,409', N'785,12
0,445', N'1064,8
0,479', N'1397,5
0,511', N'1798,1
0,543', N'2244,4
0,573', N'3375,9
0,632')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (10, 2.6, N'0,021709
0,035', N'0,047416
0,043', N'0,084425
0,049', N'0,16373
0,058', N'3,3908
0,123', N'1,3103
0,097', N'6,1165
0,142', N'9,8623
0,159', N'16,987
0,182', N'23,152
0,196', N'41,793
0,226', N'61,779
0,249', N'99,127
0,279', N'138,59
0,302', N'144,11
0,305', N'196,95
0,329', N'254,91
0,35', N'403,72
0,391', N'575,44
0,426', N'818,64
0,464', N'1109,3
0,499', N'1457,6
0,533', N'1874,3
0,566', N'2338,4
0,597', N'3514,8
0,658')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (11, 2.8, N'0,02295
0,037', N'0,048519
0,044', N'0,087871
0,051', N'0,1722
0,061', N'3,5011
0,127', N'1,3643
0,101', N'6,3319
0,147', N'10,296
0,166', N'17,64
0,189', N'24,097
0,204', N'43,457
0,235', N'64,012
0,258', N'102,68
0,289', N'144,09
0,314', N'149,78
0,317', N'204,74
0,342', N'265,11
0,364', N'419,2
0,406', N'597,06
0,442', N'848,64
0,481', N'1151,5
0,518', N'1512,3
0,553', N'1943,8
0,587', N'2424,6
0,619', N'3648,3
0,683')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (12, 3, N'0,02357
0,038', N'0,050724
0,046', N'0,091317
0,053', N'0,17784
0,063', N'3,6389
0,132', N'1,4183
0,105', N'6,5903
0,153', N'10,669
0,172', N'18,293
0,196', N'24,924
0,211', N'44,937
0,243', N'66,245
0,267', N'106,59
0,3', N'149,14
0,325', N'154,98
0,328', N'211,92
0,354', N'274,58
0,377', N'434,69
0,421', N'618,67
0,458', N'880,39
0,499', N'1191,6
0,536', N'1567
0,573', N'2013,4
0,608', N'2510,8
0,641', N'3776,5
0,707')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (13, 3.2, N'0,02419
0,039', N'0,05293
0,048', N'0,094763
0,055', N'0,18349
0,065', N'3,7767
0,137', N'1,4589
0,108', N'6,8057
0,158', N'10,979
0,177', N'18,853
0,202', N'25,751
0,218', N'46,601
0,252', N'68,478
0,276', N'110,14
0,31', N'154,19
0,336', N'160,18
0,339', N'219,1
0,366', N'283,32
0,389', N'449,15
0,435', N'638,93
0,473', N'908,62
0,515', N'1231,6
0,554', N'1619
0,592', N'2079,6
0,628', N'2596,9
0,663', N'3904,7
0,731')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (14, 3.4, N'0,025431
0,041', N'0,054032
0,049', N'0,098209
0,057', N'0,18913
0,067', N'3,887
0,141', N'1,5129
0,112', N'7,0211
0,163', N'11,351
0,183', N'19,506
0,209', N'26,578
0,225', N'48,081
0,26', N'70,711
0,285', N'113,69
0,32', N'159,24
0,347', N'165,37
0,35', N'225,69
0,377', N'292,06
0,401', N'462,57
0,448', N'659,19
0,488', N'936,85
0,531', N'1269,4
0,571', N'1668,2
0,61', N'2145,8
0,648', N'2675,3
0,683', N'4022,3
0,753')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (15, 3.6, N'0,026051
0,042', N'0,056238
0,051', N'0,10166
0,059', N'0,19478
0,069', N'3,9973
0,145', N'1,5534
0,115', N'7,2365
0,168', N'11,661
0,188', N'20,066
0,215', N'27,405
0,232', N'49,375
0,267', N'72,695
0,293', N'116,89
0,329', N'163,83
0,357', N'170,1
0,36', N'232,27
0,388', N'300,8
0,413', N'475,99
0,461', N'678,1
0,502', N'965,08
0,547', N'1307,2
0,588', N'1717,4
0,628', N'2208,7
0,667', N'2753,6
0,703', N'4139,8
0,775')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (16, 3.8, N'0,026672
0,043', N'0,05734
0,052', N'0,10338
0,06', N'0,20043
0,071', N'4,1076
0,149', N'1,6075
0,119', N'7,4088
0,172', N'12,033
0,194', N'20,626
0,221', N'28,113
0,238', N'50,854
0,275', N'74,928
0,302', N'120,09
0,338', N'168,42
0,367', N'174,82
0,37', N'238,86
0,399', N'309,54
0,425', N'489,42
0,474', N'697,02
0,516', N'991,55
0,562', N'1342,7
0,604', N'1763,9
0,645', N'2268,3
0,685', N'2832
0,723', N'4257,3
0,797')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (17, 4, N'0,027292
0,044', N'0,059546
0,054', N'0,10682
0,062', N'0,20607
0,073', N'4,2178
0,153', N'1,648
0,122', N'7,6241
0,177', N'12,343
0,199', N'21,186
0,227', N'28,822
0,244', N'52,149
0,282', N'76,913
0,31', N'123,29
0,347', N'172,55
0,376', N'179,55
0,38', N'245,44
0,41', N'317,55
0,436', N'502,84
0,487', N'715,93
0,53', N'1018
0,577', N'1378,3
0,62', N'1810,4
0,662', N'2327,9
0,703', N'2902,5
0,741', N'4369,5
0,818')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (18, 5, N'0,031013
0,05', N'0,066162
0,06', N'0,12061
0,07', N'0,23148
0,082', N'4,7416
0,172', N'1,8506
0,137', N'8,5287
0,198', N'13,832
0,223', N'23,706
0,254', N'32,366
0,274', N'58,436
0,316', N'86,093
0,347', N'138,21
0,389', N'193,66
0,422', N'201,28
0,426', N'274,78
0,459', N'355,42
0,488', N'562,73
0,545', N'801,03
0,593', N'1138
0,645', N'1542,8
0,694', N'2026,4
0,741', N'2606,1
0,787', N'3251,1
0,83', N'4887,6
0,915')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (19, 6, N'0,034115
0,055', N'0,072778
0,066', N'0,13267
0,077', N'0,25406
0,09', N'5,2103
0,189', N'2,0262
0,15', N'9,3902
0,218', N'15,197
0,245', N'26,04
0,279', N'35,555
0,301', N'64,169
0,347', N'94,529
0,381', N'151,35
0,426', N'212,01
0,462', N'220,66
0,467', N'301,12
0,503', N'389,65
0,535', N'616,42
0,597', N'878,02
0,65', N'1247,4
0,707', N'1691,7
0,761', N'2220,6
0,812', N'2857,8
0,863', N'3560,5
0,909', N'5357,7
1')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (20, 7, N'0,037216
0,06', N'0,079394
0,072', N'0,14301
0,083', N'0,27665
0,098', N'5,6238
0,204', N'2,2018
0,163', N'10,166
0,236', N'16,437
0,265', N'28,186
0,302', N'38,39
0,325', N'69,347
0,375', N'101,97
0,411', N'163,79
0,461', N'229,45
0,5', N'238,61
0,505', N'325,66
0,544', N'420,97
0,578', N'667,01
0,646', N'949,62
0,703', N'1349,7
0,765', N'1827,3
0,822', N'2401,1
0,878', N'3086,3
0,932', N'3850,4
0,983', N'5790,3
1,08')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (21, 8, N'0,039697
0,064', N'0,084908
0,077', N'0,15334
0,089', N'0,29641
0,105', N'6,0373
0,219', N'2,3504
0,174', N'10,855
0,252', N'17,616
0,284', N'30,146
0,323', N'41,107
0,348', N'74,155
0,401', N'109,17
0,44', N'175,16
0,493', N'245,51
0,535', N'255,15
0,54', N'348,41
0,582', N'450,83
0,619', N'713,47
0,691', N'1015,8
0,752', N'1443,2
0,818', N'1956,3
0,88', N'2567,9
0,939', N'3301,5
0,997', N'4116,7
1,05', N'6191
1,16')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (22, 9, N'0,042178
0,068', N'0,090421
0,082', N'0,16368
0,095', N'0,31617
0,112', N'6,3957
0,232', N'2,499
0,185', N'11,544
0,268', N'18,67
0,301', N'32,013
0,343', N'43,588
0,369', N'78,778
0,426', N'115,87
0,467', N'186,17
0,524', N'260,65
0,568', N'270,74
0,573', N'369,36
0,617', N'478,51
0,657', N'756,84
0,733', N'1077,9
0,798', N'1531,4
0,868', N'2074,1
0,933', N'2723,8
0,996', N'3503,5
1,06', N'4367,4
1,11', N'6564,9
1,23')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (23, 10, N'0,044659
0,072', N'0,095935
0,087', N'0,1723
0,1', N'0,3331
0,118', N'6,754
0,245', N'2,6341
0,195', N'12,19
0,283', N'19,725
0,318', N'33,786
0,362', N'46,068
0,39', N'83,031
0,449', N'122,32
0,493', N'196,12
0,552', N'274,88
0,599', N'285,39
0,604', N'389,72
0,651', N'504,73
0,693', N'798,14
0,773', N'1136
0,841', N'1614,4
0,915', N'2187,5
0,984', N'2871,5
1,05', N'3692,3
1,11', N'4606,3
1,18', N'6922,8
1,3')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (24, 12, N'0,049001
0,079', N'0,10476
0,095', N'0,18953
0,11', N'0,36698
0,13', N'7,4157
0,269', N'2,8907
0,214', N'13,353
0,31', N'21,585
0,348', N'37,053
0,397', N'50,439
0,427', N'91,168
0,493', N'134,23
0,541', N'214,95
0,605', N'301,04
0,656', N'312,79
0,662', N'427,43
0,714', N'552,8
0,759', N'874,55
0,847', N'1245,4
0,922', N'1769,6
1', N'2398,7
1,08', N'3147,7
1,15', N'4049,9
1,22', N'5049
1,29', N'7590,5
1,42')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (25, 14, N'0,053343
0,086', N'0,11358
0,103', N'0,20676
0,12', N'0,39521
0,14', N'8,0221
0,291', N'3,1339
0,232', N'14,473
0,336', N'23,384
0,377', N'40,04
0,429', N'54,573
0,462', N'98,565
0,533', N'144,89
0,584', N'232,36
0,654', N'325,36
0,709', N'338,31
0,716', N'461,55
0,771', N'597,95
0,821', N'945,79
0,916', N'1346,8
0,997', N'1912,5
1,08', N'2592,1
1,17', N'3402
1,24', N'4374,4
1,32', N'5456,3
1,39', N'8199,4
1,53')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (26, 16, N'0,057065
0,092', N'0,1224
0,111', N'0,22054
0,128', N'0,42344
0,15', N'8,6011
0,312', N'3,35
0,248', N'15,464
0,359', N'24,997
0,403', N'42,84
0,459', N'58,471
0,495', N'105,41
0,57', N'155,07
0,625', N'248,7
0,7', N'348,3
0,759', N'361,93
0,766', N'493,88
0,825', N'639,47
0,878', N'1011,9
0,98', N'1440
1,07', N'2044,8
1,16', N'2772,1
1,25', N'3637,2
1,33', N'4679,1
1,41', N'5832,4
1,49', N'8765,6
1,64')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (27, 18, N'0,060786
0,098', N'0,13012
0,118', N'0,23432
0,136', N'0,45167
0,16', N'9,1248
0,331', N'3,5661
0,264', N'16,411
0,381', N'26,547
0,428', N'45,453
0,487', N'62,015
0,525', N'111,88
0,605', N'164,5
0,663', N'263,98
0,743', N'369,41
0,805', N'384,14
0,813', N'524,41
0,876', N'678,07
0,931', N'1072,8
1,04', N'1527,8
1,13', N'2170,1
1,23', N'2938,9
1,32', N'3858,7
1,41', N'4963,9
1,5', N'6188,8
1,58', N'9299,8
1,74')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (28, 20, N'0,063888
0,103', N'0,13673
0,124', N'0,24811
0,144', N'0,47707
0,169', N'9,621
0,349', N'3,7552
0,278', N'17,316
0,402', N'28,036
0,452', N'47,973
0,514', N'65,441
0,554', N'117,98
0,638', N'173,68
0,7', N'278,19
0,783', N'389,61
0,849', N'404,93
0,857', N'552,55
0,923', N'715,21
0,982', N'1131,6
1,1', N'1610,2
1,19', N'2288,3
1,3', N'3098,9
1,39', N'4069,3
1,49', N'5232,1
1,58', N'6525,7
1,67', N'9807,3
1,84')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (29, 24, N'0,070711
0,114', N'0,15107
0,137', N'0,27223
0,158', N'0,52224
0,185', N'10,558
0,383', N'4,12
0,305', N'18,996
0,441', N'30,703
0,495', N'52,546
0,563', N'71,701
0,607', N'129,26
0,699', N'190,3
0,767', N'305,2
0,859', N'427,24
0,931', N'443,67
0,939', N'605,83
1,01', N'783,67
1,08', N'1240,1
1,2', N'1765,5
1,31', N'2507,1
1,42', N'3396,8
1,53', N'4460,4
1,63', N'5735,4
1,73', N'7148,5
1,82', N'10747
2,01')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (30, 28, N'0,076293
0,123', N'0,1632
0,148', N'0,29463
0,171', N'0,56458
0,2', N'11,413
0,414', N'4,4577
0,33', N'20,546
0,477', N'33,184
0,535', N'56,839
0,609', N'77,489
0,656', N'139,8
0,756', N'205,68
0,829', N'329,71
0,928', N'461,65
1,01', N'479,58
1,01', N'654,92
1,09', N'847,04
1,16', N'1340,2
1,3', N'1907,3
1,41', N'2710
1,54', N'3670,3
1,65', N'4818,6
1,76', N'6195,7
1,87', N'7724,3
1,97', N'11607
2,17')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (31, 32, N'0,081875
0,132', N'0,17533
0,159', N'0,3153
0,183', N'0,60693
0,215', N'12,212
0,443', N'4,7819
0,354', N'21,968
0,51', N'35,541
0,573', N'60,853
0,652', N'82,923
0,702', N'149,42
0,808', N'220,07
0,887', N'352,81
0,993', N'493,78
1,08', N'513,13
1,09', N'700,41
1,17', N'906,03
1,24', N'1433,1
1,39', N'2039,7
1,51', N'2897
1,64', N'3923,7
1,76', N'5152,2
1,88', N'6622,9
2', N'8260,9
2,11', N'12414
2,32')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (32, 36, N'0,086838
0,14', N'0,18636
0,169', N'0,33598
0,195', N'0,64362
0,228', N'12,957
0,47', N'5,0655
0,375', N'23,346
0,542', N'37,712
0,608', N'64,493
0,691', N'88,002
0,745', N'158,67
0,858', N'233,47
0,941', N'374,12
1,05', N'523,6
1,14', N'544,31
1,15', N'742,92
1,24', N'961,38
1,32', N'1519,9
1,47', N'2164
1,6', N'3073,4
1,74', N'4163,8
1,87', N'5466,7
2', N'7026,9
2,12', N'8762,2
2,24', N'13167
2,46')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (33, 40, N'0,0918
0,148', N'0,19628
0,178', N'0,35321
0,205', N'0,68032
0,241', N'13,673
0,496', N'5,3492
0,396', N'24,595
0,571', N'39,759
0,641', N'68,039
0,729', N'92,727
0,785', N'167,17
0,904', N'246,12
0,992', N'394,73
1,11', N'552,51
1,2', N'574,08
1,21', N'783,03
1,31', N'1013,1
1,39', N'1602,5
1,55', N'2281,5
1,69', N'3241,1
1,84', N'4390,5
1,97', N'5762,1
2,11', N'7407,7
2,24', N'9236,2
2,36', N'13883
2,6')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (34, 50, N'0,10296
0,166', N'0,22054
0,2', N'0,39628
0,23', N'0,76219
0,27', N'15,328
0,556', N'5,9841
0,443', N'27,567
0,64', N'44,473
0,717', N'76,159
0,816', N'103,83
0,879', N'187,14
1,01', N'275,4
1,11', N'441,27
1,24', N'617,68
1,35', N'642,12
1,36', N'875,82
1,46', N'1133,3
1,56', N'1792,5
1,74', N'2551,7
1,89', N'3623,9
2,05', N'4908,5
2,21', N'6445,8
2,36', N'8285,2
2,5', N'10329
2,64', N'15523
2,91')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (35, 60, N'0,11289
0,182', N'0,24149
0,219', N'0,43591
0,253', N'0,83558
0,296', N'16,789
0,609', N'6,5649
0,486', N'30,195
0,701', N'48,815
0,787', N'83,439
0,894', N'113,75
0,963', N'205,08
1,11', N'301,7
1,22', N'483,91
1,36', N'676,88
1,47', N'703,55
1,49', N'960,23
1,6', N'1242,5
1,71', N'1964,9
1,9', N'2796,2
2,07', N'3971,5
2,25', N'5379,8
2,42', N'7061,1
2,58', N'9080
2,74', N'11320
2,89', N'17008
3,18')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (36, 70, N'0,12219
0,197', N'0,26134
0,237', N'0,47037
0,273', N'0,90333
0,32', N'18,139
0,658', N'7,1052
0,526', N'32,65
0,758', N'52,723
0,85', N'90,252
0,967', N'122,97
1,04', N'221,54
1,2', N'326,01
1,31', N'522,64
1,47', N'731,49
1,59', N'760,25
1,61', N'1037,5
1,73', N'1342,3
1,84', N'2122,9
2,06', N'3020,4
2,24', N'4290,8
2,43', N'5811,1
2,61', N'7627,2
2,79', N'9808,5
2,96', N'12229
3,12', N'18375
3,44')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (37, 80, N'0,13088
0,211', N'0,28009
0,254', N'0,50483
0,293', N'0,96826
0,343', N'19,408
0,704', N'7,5915
0,562', N'34,933
0,811', N'56,382
0,909', N'96,506
1,03', N'131,47
1,11', N'237,07
1,28', N'348,59
1,4', N'558,87
1,57', N'782,42
1,7', N'812,69
1,72', N'1109,3
1,85', N'1434,8
1,97', N'2269,5
2,2', N'3229,8
2,39', N'4587,2
2,6', N'6213,4
2,79', N'8155
2,98', N'10487
3,17', N'13075
3,34', N'19647
3,68')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (38, 90, N'0,13894
0,224', N'0,29773
0,27', N'0,53584
0,311', N'1,0275
0,364', N'20,593
0,747', N'8,0643
0,597', N'37,044
0,86', N'59,856
0,965', N'102,39
1,1', N'139,5
1,18', N'251,5
1,36', N'369,93
1,49', N'592,98
1,67', N'829,69
1,81', N'862,3
1,82', N'1176,9
1,97', N'1522,2
2,09', N'2407,8
2,33', N'3427
2,54', N'4866
2,76', N'6591,3
2,96', N'8652,7
3,16', N'11123
3,36', N'13866
3,54', N'20843
3,9')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (39, 100, N'0,147
0,237', N'0,31427
0,285', N'0,56513
0,328', N'1,084
0,384', N'21,723
0,788', N'8,4966
0,629', N'39,068
0,907', N'63,081
1,02', N'107,99
1,16', N'147,06
1,24', N'265,18
1,43', N'390,02
1,57', N'625,31
1,76', N'875,12
1,91', N'909,08
1,92', N'1240,4
2,07', N'1605,2
2,2', N'2537,9
2,46', N'3612,1
2,67', N'5130,6
2,91', N'6949,3
3,13', N'9120,4
3,33', N'11726
3,54', N'14618
3,73', N'21970
4,11')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (40, 120, N'0,16127
0,26', N'0,34404
0,312', N'0,62027
0,36', N'1,1884
0,421', N'23,818
0,864', N'9,3206
0,69', N'42,816
0,994', N'69,16
1,11', N'118,35
1,27', N'161,24
1,36', N'290,52
1,57', N'427,49
1,72', N'685
1,93', N'958,64
2,09', N'996,02
2,11', N'1359,5
2,27', N'1758,9
2,41', N'2781,6
2,69', N'3957,9
2,93', N'5621,1
3,19', N'7614
3,42', N'9992,7
3,65', N'12848
3,88', N'16016
4,09', N'24069
4,51')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (41, 140, N'0,1743
0,281', N'0,37271
0,338', N'0,67023
0,389', N'1,2844
0,455', N'25,748
0,934', N'10,077
0,746', N'46,262
1,07', N'74,742
1,2', N'127,87
1,37', N'174,23
1,47', N'313,82
1,7', N'461,73
1,86', N'740,07
2,08', N'1035,7
2,26', N'1076,3
2,28', N'1468,5
2,45', N'1900,2
2,61', N'3004,6
2,91', N'4275,3
3,16', N'6071
3,44', N'8225,3
3,7', N'10797
3,95', N'13878
4,19', N'17301
4,42', N'26003
4,87')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (42, 160, N'0,1867
0,301', N'0,39807
0,361', N'0,71675
0,416', N'1,3748
0,487', N'27,54
0,999', N'10,779
0,798', N'49,492
1,15', N'79,891
1,29', N'136,73
1,46', N'186,28
1,58', N'335,64
1,81', N'493,73
1,99', N'791,24
2,23', N'1107,3
2,41', N'1151
2,44', N'1570,2
2,62', N'2031,3
2,79', N'3212,2
3,11', N'4571,1
3,38', N'6492,7
3,68', N'8794,4
3,96', N'11543
4,22', N'14839
4,48', N'18500
4,72', N'27798
5,2')
INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] ([id], [r], [15], [20], [25], [32], [100], [70], [125], [150], [184], [207], [259], [300], [359], [408], [414], [466], [514], [612], [700], [800], [898], [996], [1096], [1192], [1392]) VALUES (43, 180, N'0,19849
0,32', N'0,42344
0,384', N'0,76155
0,442', N'1,4594
0,517', N'29,194
1,06', N'11,441
0,847', N'52,507
1,22', N'84,791
1,37', N'145,04
1,55', N'197,62
1,67', N'356,17
1,93', N'523,75
2,11', N'839,56
2,36', N'1174,8
2,56', N'1220,9
2,58', N'1665,4
2,78', N'2155,1
2,96', N'3407,3
3,3', N'4849,4
3,59', N'6886,1
3,9', N'9327,9
4,2', N'12243
4,48', N'15739
4,75', N'19624
5,01', N'29486
5,52')
SET IDENTITY_INSERT [dbo].[19_tablitsa_dlya_gidravlicheskogo_rascheta_truboprovodov] OFF
GO

SET IDENTITY_INSERT [dbo].[23_popravochnyy_koeff_rascheta_otopitelnoy_nagruzki_zdaniya] ON 

INSERT [dbo].[23_popravochnyy_koeff_rascheta_otopitelnoy_nagruzki_zdaniya] ([id], [nomer], [tn], [alfa]) VALUES (1, 1, 0, 2.05)
INSERT [dbo].[23_popravochnyy_koeff_rascheta_otopitelnoy_nagruzki_zdaniya] ([id], [nomer], [tn], [alfa]) VALUES (2, 2, -5, 1.67)
INSERT [dbo].[23_popravochnyy_koeff_rascheta_otopitelnoy_nagruzki_zdaniya] ([id], [nomer], [tn], [alfa]) VALUES (3, 3, -10, 1.45)
INSERT [dbo].[23_popravochnyy_koeff_rascheta_otopitelnoy_nagruzki_zdaniya] ([id], [nomer], [tn], [alfa]) VALUES (4, 4, -15, 1.29)
INSERT [dbo].[23_popravochnyy_koeff_rascheta_otopitelnoy_nagruzki_zdaniya] ([id], [nomer], [tn], [alfa]) VALUES (5, 5, -20, 1.17)
INSERT [dbo].[23_popravochnyy_koeff_rascheta_otopitelnoy_nagruzki_zdaniya] ([id], [nomer], [tn], [alfa]) VALUES (6, 6, -25, 1.08)
INSERT [dbo].[23_popravochnyy_koeff_rascheta_otopitelnoy_nagruzki_zdaniya] ([id], [nomer], [tn], [alfa]) VALUES (7, 7, -30, 1)
INSERT [dbo].[23_popravochnyy_koeff_rascheta_otopitelnoy_nagruzki_zdaniya] ([id], [nomer], [tn], [alfa]) VALUES (8, 8, -35, 1.17)
INSERT [dbo].[23_popravochnyy_koeff_rascheta_otopitelnoy_nagruzki_zdaniya] ([id], [nomer], [tn], [alfa]) VALUES (9, 9, -40, 0.9)
INSERT [dbo].[23_popravochnyy_koeff_rascheta_otopitelnoy_nagruzki_zdaniya] ([id], [nomer], [tn], [alfa]) VALUES (10, 10, -45, 0.85)
INSERT [dbo].[23_popravochnyy_koeff_rascheta_otopitelnoy_nagruzki_zdaniya] ([id], [nomer], [tn], [alfa]) VALUES (11, 11, -50, 0.82)
INSERT [dbo].[23_popravochnyy_koeff_rascheta_otopitelnoy_nagruzki_zdaniya] ([id], [nomer], [tn], [alfa]) VALUES (12, 12, -55, 0.8)
SET IDENTITY_INSERT [dbo].[23_popravochnyy_koeff_rascheta_otopitelnoy_nagruzki_zdaniya] OFF
GO

SET IDENTITY_INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ON 

INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (1, 1, N'Радиаторы чугунные высотой  500мм', N'95-70', 19.5)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (2, 2, N'Радиаторы чугунные высотой  500мм', N'110-70', 17.6)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (3, 3, N'Радиаторы чугунные высотой  500мм', N'130-70', 15.1)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (4, 4, N'Радиаторы чугунные высотой  500мм', N'140-70', 14.6)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (5, 5, N'Радиаторы чугунные высотой  500мм', N'150-70', 13.3)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (6, 6, N'Радиаторы чугунные высотой  1000мм', N'95-70', 31)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (7, 7, N'Радиаторы чугунные высотой  1000мм', N'110-70', 28.2)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (8, 8, N'Радиаторы чугунные высотой  1000мм', N'130-70', 24.2)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (9, 9, N'Радиаторы чугунные высотой  1000мм', N'140-70', 23.2)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (10, 10, N'Радиаторы чугунные высотой  1000мм', N'150-70', 21.6)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (11, 11, N'Трубы чугунные ребристые', N'95-70', 14.2)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (12, 12, N'Трубы чугунные ребристые', N'110-70', 12.5)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (13, 13, N'Трубы чугунные ребристые', N'130-70', 10.8)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (14, 14, N'Трубы чугунные ребристые', N'140-70', 10.4)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (15, 15, N'Трубы чугунные ребристые', N'150-70', 9.2)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (16, 16, N'Конвертоми плинтусовые', N'95-70', 5.6)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (17, 17, N'Конвертоми плинтусовые', N'110-70', 5)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (18, 18, N'Конвертоми плинтусовые', N'130-70', 4.3)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (19, 19, N'Конвертоми плинтусовые', N'140-70', 4.1)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (20, 20, N'Конвертоми плинтусовые', N'150-70', 3.7)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (21, 21, N'Регистры из стальных труб', N'95-70', 37)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (22, 22, N'Регистры из стальных труб', N'110-70', 32)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (23, 23, N'Регистры из стальных труб', N'130-70', 27)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (24, 24, N'Регистры из стальных труб', N'140-70', 26)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (25, 25, N'Регистры из стальных труб', N'150-70', 24)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (26, 26, N'Радиаторы стальные листотрубные', N'95-70', 5.6)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (27, 27, N'Радиаторы стальные листотрубные', N'110-70', 5)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (28, 28, N'Радиаторы стальные листотрубные', N'130-70', 4.3)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (29, 29, N'Радиаторы стальные листотрубные', N'140-70', 4.1)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (30, 30, N'Радиаторы стальные листотрубные', N'150-70', 3.7)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (31, 31, N'Радиаторы стальные панельные высотой 500мм', N'95-70', 11.7)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (32, 32, N'Радиаторы стальные панельные высотой 500мм', N'110-70', 10.6)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (33, 33, N'Радиаторы стальные панельные высотой 500мм', N'130-70', 9.1)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (34, 34, N'Радиаторы стальные панельные высотой 500мм', N'140-70', 8.8)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (35, 35, N'Радиаторы стальные панельные высотой 500мм', N'150-70', 8)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (36, 36, N'Радиаторы стальные панельные высотой 350мм', N'95-70', 10)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (37, 37, N'Радиаторы стальные панельные высотой 350мм', N'110-70', 9)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (38, 38, N'Радиаторы стальные панельные высотой 350мм', N'130-70', 7.8)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (39, 39, N'Радиаторы стальные панельные высотой 350мм', N'140-70', 7.5)
INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (40, 40, N'Радиаторы стальные панельные высотой 350мм', N'150-70', 6.8)
SET IDENTITY_INSERT [dbo].[24_udelnyy_obem_vody_v_sisteme_otopleniya] OFF
GO

SET IDENTITY_INSERT [dbo].[26_mesto_prokladki_uchastka_truboprovoda] ON 

INSERT [dbo].[26_mesto_prokladki_uchastka_truboprovoda] ([id], [mesto_prokladki]) VALUES (1, N'Тротуар')
INSERT [dbo].[26_mesto_prokladki_uchastka_truboprovoda] ([id], [mesto_prokladki]) VALUES (2, N'Проезжая часть')
SET IDENTITY_INSERT [dbo].[26_mesto_prokladki_uchastka_truboprovoda] OFF
GO

SET IDENTITY_INSERT [dbo].[27_vid_pokrytiya_uchastka_teploprovoda] ON 

INSERT [dbo].[27_vid_pokrytiya_uchastka_teploprovoda] ([id], [vid_pokrit]) VALUES (1, N'Асфальт')
INSERT [dbo].[27_vid_pokrytiya_uchastka_teploprovoda] ([id], [vid_pokrit]) VALUES (2, N'Глина')
SET IDENTITY_INSERT [dbo].[27_vid_pokrytiya_uchastka_teploprovoda] OFF
GO

SET IDENTITY_INSERT [dbo].[28_koeffitsienty_rascheta_balansovoy_nagruzki_gv] ON 

INSERT [dbo].[28_koeffitsienty_rascheta_balansovoy_nagruzki_gv] ([id], [pr], [sm], [ps], [pw], [o]) VALUES (1, 1.15, 1.1, 1.25, 1.25, 1)
SET IDENTITY_INSERT [dbo].[28_koeffitsienty_rascheta_balansovoy_nagruzki_gv] OFF
GO
SET IDENTITY_INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ON 

INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (1, N'Жилое здание', 150, 5.15)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (2, N'Жилое здание', 25, 4.3)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (3, N'Жилое здание', 350, 4.1)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (4, N'Жилое здание', 500, 3.75)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (5, N'Жилое здание', 700, 3.5)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (6, N'Жилое здание', 1000, 3.27)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (7, N'Жилое здание', 2000, 2.97)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (8, N'Жилое здание', 3000, 2.85)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (9, N'Жилое здание', 4000, 2.78)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (10, N'Жилое здание', 6000, 2.7)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (11, N'Жилое здание', 10000, 2.6)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (12, N'Жилое здание', 20000, 2.4)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (13, N'Санаторий с ваннами', 0, 1)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (14, N'Ясли-сад', 0, 5.43)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (15, N'Школа, с душевыми, столовой', 0, 6.86)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (16, N'Гостиница, общий душ, ванна', 0, 2.81)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (17, N'Жилое здание, с душевыми', 0, 1.9)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (18, N'Жилое здание, сидячие ванны', 0, 2.01)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (19, N'Жилое здание, ванны длиной 1.5-1.7м', 0, 2)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (20, N'Жилое здание, высотой >12 этажей', 0, 2.01)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (21, N'Общежитие, с общими душевыми', 0, 2.52)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (22, N'Общежитие, отдельные душевые', 0, 2.81)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (23, N'Общежитие, общие кухни, душевые на этажах', 0, 2)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (24, N'Больница', 0, 1.73)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (25, N'Больница инфекционная', 0, 2.07)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (26, N'Санаторий с душами, ваннами', 0, 2.62)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (27, N'Поликлиника', 0, 4.47)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (28, N'Ясли-сады, круклосуточные', 0, 4.8)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (29, N'Учебн.заведение, с душевыми, буфетам', 0, 3.6)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (30, N'Школа-интернат', 0, 4.8)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (31, N'Театр', 0, 1.44)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (32, N'Административное здание', 0, 6.86)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (33, N'Гостиница, отдельные душевые', 0, 2.06)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (34, N'Гостиница, отдельные ванны до 25%', 0, 2.5)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (35, N'Гостиница, отдельные ванны до 100%', 0, 2.16)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (36, N'Парихмахерские', 0, 3.22)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (37, N'Магазин промтоварный', 0, 6.86)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (38, N'Магазин продовольственный', 0, 4)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (39, N'Стадион,  с душевыми', 0, 2)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (40, N'Спортивный зал, с душевыми', 0, 2)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (41, N'Бассейн, с душевыми', 0, 2)
INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] ([id], [name_zd], [populace], [gvs_max]) VALUES (42, N'Предприятие, с небольшим тепловыделением', 0, 8.4)
SET IDENTITY_INSERT [dbo].[29_koeff_chasovoy_neravnomernosti_nagruzki_gv__sp_41_101_95] OFF
GO
SET IDENTITY_INSERT [dbo].[30_koeffitsienty_mestnyh_teplovyh_poter] ON 

INSERT [dbo].[30_koeffitsienty_mestnyh_teplovyh_poter] ([id], [pr], [diametr], [beta_mag], [beta_rasp]) VALUES (1, N'Подземная бесканальная', 150, 1.15, 1.15)
INSERT [dbo].[30_koeffitsienty_mestnyh_teplovyh_poter] ([id], [pr], [diametr], [beta_mag], [beta_rasp]) VALUES (2, N'Подземная в каналах', 150, 1.15, 1.2)
INSERT [dbo].[30_koeffitsienty_mestnyh_teplovyh_poter] ([id], [pr], [diametr], [beta_mag], [beta_rasp]) VALUES (3, N'Надземная', 150, 1.15, 1.2)
SET IDENTITY_INSERT [dbo].[30_koeffitsienty_mestnyh_teplovyh_poter] OFF
GO
SET IDENTITY_INSERT [dbo].[31_udelnyy_obem_vody_v_sisteme_ventilyatsii] ON 

INSERT [dbo].[31_udelnyy_obem_vody_v_sisteme_ventilyatsii] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (1, 31, N'Калориферное  оборудование', N'95-70', 8.5)
INSERT [dbo].[31_udelnyy_obem_vody_v_sisteme_ventilyatsii] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (2, 32, N'Калориферное  оборудование', N'110-70', 7.5)
INSERT [dbo].[31_udelnyy_obem_vody_v_sisteme_ventilyatsii] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (3, 33, N'Калориферное  оборудование', N'130-70', 6.5)
INSERT [dbo].[31_udelnyy_obem_vody_v_sisteme_ventilyatsii] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (4, 34, N'Калориферное  оборудование', N'140-70', 6)
INSERT [dbo].[31_udelnyy_obem_vody_v_sisteme_ventilyatsii] ([id], [nomer], [typ_co], [delta_t], [q_udel]) VALUES (5, 35, N'Калориферное  оборудование', N'150-70', 5.5)
SET IDENTITY_INSERT [dbo].[31_udelnyy_obem_vody_v_sisteme_ventilyatsii] OFF
GO
SET IDENTITY_INSERT [dbo].[36_naimenovanie_rayona_mestonahozhdeniya_teploprovoda] ON 

INSERT [dbo].[36_naimenovanie_rayona_mestonahozhdeniya_teploprovoda] ([id], [name_typ]) VALUES (1, N'Город')
INSERT [dbo].[36_naimenovanie_rayona_mestonahozhdeniya_teploprovoda] ([id], [name_typ]) VALUES (2, N'НС №1')
INSERT [dbo].[36_naimenovanie_rayona_mestonahozhdeniya_teploprovoda] ([id], [name_typ]) VALUES (3, N'НС №2')
INSERT [dbo].[36_naimenovanie_rayona_mestonahozhdeniya_teploprovoda] ([id], [name_typ]) VALUES (4, N'НС №3')
INSERT [dbo].[36_naimenovanie_rayona_mestonahozhdeniya_teploprovoda] ([id], [name_typ]) VALUES (5, N'НС №4')
INSERT [dbo].[36_naimenovanie_rayona_mestonahozhdeniya_teploprovoda] ([id], [name_typ]) VALUES (6, N'НС №5')
INSERT [dbo].[36_naimenovanie_rayona_mestonahozhdeniya_teploprovoda] ([id], [name_typ]) VALUES (7, N'НС №6')
INSERT [dbo].[36_naimenovanie_rayona_mestonahozhdeniya_teploprovoda] ([id], [name_typ]) VALUES (8, N'НС №7')
INSERT [dbo].[36_naimenovanie_rayona_mestonahozhdeniya_teploprovoda] ([id], [name_typ]) VALUES (9, N'Пром.зона')
INSERT [dbo].[36_naimenovanie_rayona_mestonahozhdeniya_teploprovoda] ([id], [name_typ]) VALUES (10, N'Северное шоссе')
INSERT [dbo].[36_naimenovanie_rayona_mestonahozhdeniya_teploprovoda] ([id], [name_typ]) VALUES (11, N'ЦТП 1')
INSERT [dbo].[36_naimenovanie_rayona_mestonahozhdeniya_teploprovoda] ([id], [name_typ]) VALUES (12, N'ЦТП 2')
SET IDENTITY_INSERT [dbo].[36_naimenovanie_rayona_mestonahozhdeniya_teploprovoda] OFF
GO
SET IDENTITY_INSERT [dbo].[39_normy_teplovyh_poter] ON 

INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (1, 18, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 22, 17, 28, 17, 33, 17, 33, 17, 22, 17, 28, 17, 33, 17, 33, 17)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (2, 25, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 24, 18, 30, 18, 35, 18, 35, 18, 24, 18, 30, 18, 35, 18, 35, 18)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (3, 32, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 25, 20, 32, 20, 38, 20, 38, 20, 25, 20, 32, 20, 38, 20, 38, 20)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (4, 38, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 26, 21, 34, 21, 40, 21, 40, 21, 26, 21, 34, 21, 40, 21, 40, 21)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (5, 45, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 28, 22, 36, 22, 42, 22, 42, 22, 28, 22, 36, 22, 42, 22, 42, 22)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (6, 57, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 31, 25, 40, 25, 47, 25, 47, 25, 31, 25, 40, 25, 47, 25, 47, 25)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (7, 76, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 35, 29, 45, 29, 53, 29, 53, 29, 35, 29, 45, 29, 53, 29, 53, 29)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (8, 89, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 38, 31, 49, 31, 57, 31, 57, 31, 38, 31, 49, 31, 57, 31, 57, 31)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (9, 108, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 42, 34, 54, 34, 62, 34, 62, 34, 42, 34, 54, 34, 62, 34, 62, 34)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (10, 133, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 45, 36, 59, 36, 68, 36, 68, 36, 45, 36, 59, 36, 68, 36, 68, 36)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (11, 159, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 52, 42, 65, 42, 75, 42, 75, 42, 52, 42, 65, 42, 75, 42, 75, 42)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (12, 194, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 56, 45, 74, 45, 83, 45, 83, 45, 56, 45, 74, 45, 83, 45, 83, 45)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (13, 219, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 62, 51, 79, 51, 91, 51, 91, 51, 62, 51, 79, 51, 91, 51, 91, 51)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (14, 273, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 72, 60, 90, 60, 103, 60, 103, 60, 72, 60, 90, 60, 103, 60, 103, 60)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (15, 325, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 81, 68, 100, 68, 115, 68, 115, 68, 81, 68, 100, 68, 115, 68, 115, 68)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (16, 377, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 88, 76, 107, 76, 126, 76, 126, 76, 90, 76, 107, 76, 126, 76, 126, 76)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (17, 426, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 98, 82, 121, 82, 137, 82, 137, 82, 99, 82, 121, 82, 137, 82, 137, 82)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (18, 480, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 107, 91, 132, 91, 150, 91, 150, 91, 110, 91, 132, 91, 150, 91, 150, 91)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (19, 530, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 115, 101, 142, 101, 160, 101, 160, 101, 120, 101, 142, 101, 160, 101, 160, 101)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (20, 630, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 132, 114, 163, 114, 184, 114, 184, 114, 138, 114, 163, 114, 184, 114, 184, 114)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (21, 720, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 147, 125, 181, 125, 202, 125, 202, 125, 155, 125, 181, 125, 202, 125, 202, 125)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (22, 820, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 163, 141, 200, 141, 223, 141, 223, 141, 174, 141, 200, 141, 223, 141, 223, 141)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (23, 920, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 178, 155, 218, 155, 224, 155, 224, 155, 193, 155, 218, 155, 224, 155, 224, 155)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (24, 1020, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 196, 170, 240, 170, 266, 170, 266, 170, 212, 170, 240, 170, 266, 170, 266, 170)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (25, 1220, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 229, 200, 282, 200, 308, 200, 308, 200, 229, 200, 282, 200, 308, 200, 308, 200)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (26, 1420, 0, 1, N'К', 5, 5, 50, 65, 90, 110, 110, 260, 228, 326, 228, 352, 228, 352, 228, 260, 228, 326, 228, 352, 228, 352, 228)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (27, 18, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 22, 17, 28, 17, 33, 17, 33, 17, 22, 17, 28, 17, 33, 17, 33, 17)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (28, 25, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 24, 18, 30, 18, 35, 18, 35, 18, 24, 18, 30, 18, 35, 18, 35, 18)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (29, 0, 15, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 9, NULL, 15, NULL, 24, NULL, 24, NULL, 8, NULL, 15, NULL, 21, NULL, 21, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (30, 0, 20, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 9, NULL, 18, NULL, 27, NULL, 27, NULL, 9, NULL, 16, NULL, 24, NULL, 24, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (31, 0, 25, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 10, NULL, 20, NULL, 29, NULL, 29, NULL, 9, NULL, 17, NULL, 27, NULL, 27, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (32, 0, 40, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 12, NULL, 22, NULL, 34, NULL, 34, NULL, 10, NULL, 20, NULL, 30, NULL, 30, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (33, 0, 50, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 14, NULL, 25, NULL, 37, NULL, 37, NULL, 12, NULL, 22, NULL, 33, NULL, 33, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (34, 0, 65, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 15, NULL, 28, NULL, 41, NULL, 41, NULL, 14, NULL, 25, NULL, 37, NULL, 37, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (35, 0, 80, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 17, NULL, 31, NULL, 45, NULL, 45, NULL, 15, NULL, 27, NULL, 40, NULL, 40, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (36, 0, 100, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 19, NULL, 34, NULL, 49, NULL, 49, NULL, 16, NULL, 29, NULL, 43, NULL, 43, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (37, 0, 125, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 22, NULL, 38, NULL, 54, NULL, 54, NULL, 18, NULL, 33, NULL, 47, NULL, 47, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (38, 0, 150, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 23, NULL, 41, NULL, 60, NULL, 60, NULL, 20, NULL, 36, NULL, 52, NULL, 52, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (39, 0, 200, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 29, NULL, 51, NULL, 71, NULL, 71, NULL, 24, NULL, 43, NULL, 62, NULL, 62, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (40, 0, 250, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 34, NULL, 58, NULL, 82, NULL, 82, NULL, 28, NULL, 49, NULL, 71, NULL, 71, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (41, 0, 300, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 38, NULL, 65, NULL, 91, NULL, 91, NULL, 34, NULL, 58, NULL, 82, NULL, 82, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (42, 0, 350, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 46, NULL, 79, NULL, 110, NULL, 110, NULL, 39, NULL, 66, NULL, 93, NULL, 93, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (43, 0, 400, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 52, NULL, 86, NULL, 120, NULL, 120, NULL, 42, NULL, 72, NULL, 101, NULL, 101, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (44, 0, 25, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 13, 9, 19, 9, 22, 8, 22, 8, 12, 8, 17, 8, 21, 7, 21, 7)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (45, 0, 30, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 14, 9, 20, 9, 24, 9, 24, 9, 13, 9, 17, 9, 22, 8, 22, 8)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (46, 0, 40, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 16, 10, 22, 10, 27, 9, 27, 9, 14, 9, 19, 9, 23, 9, 23, 9)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (47, 0, 50, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 16, 11, 24, 11, 29, 10, 29, 10, 15, 10, 21, 10, 26, 9, 26, 9)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (48, 0, 65, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 20, 14, 28, 12, 34, 11, 34, 11, 17, 11, 25, 11, 29, 10, 29, 10)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (49, 0, 80, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 22, 15, 30, 13, 37, 12, 37, 12, 18, 12, 27, 12, 32, 11, 32, 11)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (50, 0, 100, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 24, 16, 34, 14, 41, 14, 41, 14, 21, 14, 30, 13, 35, 12, 35, 12)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (51, 0, 125, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 25, 17, 36, 15, 45, 15, 45, 15, 22, 16, 33, 14, 37, 13, 37, 13)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (52, 0, 150, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 28, 19, 40, 16, 47, 16, 47, 16, 23, 16, 36, 15, 41, 14, 41, 14)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (53, 0, 200, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 35, 22, 47, 19, 61, 17, 61, 17, 28, 20, 42, 16, 50, 16, 50, 16)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (54, 0, 250, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 40, 26, 56, 22, 68, 18, 68, 18, 33, 22, 47, 18, 57, 17, 57, 17)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (55, 0, 300, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 46, 29, 64, 23, 76, 21, 76, 21, 37, 24, 52, 21, 61, 18, 61, 18)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (56, 0, 350, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 50, 32, 68, 25, 84, 22, 84, 22, 40, 27, 55, 22, 69, 19, 69, 19)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (57, 0, 400, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 56, 34, 75, 28, 91, 22, 91, 22, 43, 28, 60, 24, 74, 21, 74, 21)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (58, 0, 450, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 60, 36, 82, 28, 99, 23, 99, 23, 47, 31, 68, 27, 78, 22, 78, 22)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (59, 0, 500, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 65, 40, 92, 31, 112, 24, 112, 24, 50, 32, 72, 28, 86, 23, 86, 23)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (60, 0, 600, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 72, 42, 103, 33, 125, 26, 125, 26, 58, 36, 80, 30, 97, 27, 97, 27)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (61, 0, 700, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 78, 47, 120, 35, 135, 28, 135, 28, 66, 41, 92, 32, 110, 27, 110, 27)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (62, 0, 800, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 91, 53, 129, 39, 156, 31, 156, 31, 73, 44, 103, 33, 120, 29, 120, 29)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (63, 0, 900, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 101, 55, 140, 41, 172, 32, 172, 32, 78, 48, 110, 37, 129, 32, 129, 32)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (64, 0, 1000, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 111, 57, 146, 44, 183, 36, 183, 36, 86, 52, 121, 40, 141, 34, 141, 34)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (65, 0, 1200, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 135, 63, 188, 47, 220, 40, 220, 40, 98, 58, 136, 46, 164, 38, 164, 38)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (66, 0, 1400, 3, N'Б', 50, 50, 50, 65, 90, 110, 110, 149, 66, 208, 51, 236, 42, 236, 42, 112, 60, 154, 50, 193, 41, 193, 41)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (67, 0, 25, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 13, 9, 19, 9, 22, 8, 22, 8, 12, 8, 17, 8, 21, 7, 21, 7)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (68, 0, 30, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 14, 9, 20, 9, 24, 9, 24, 9, 13, 9, 17, 9, 22, 8, 22, 8)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (69, 0, 40, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 16, 10, 22, 10, 27, 9, 27, 9, 14, 9, 19, 9, 23, 9, 23, 9)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (70, 0, 50, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 16, 11, 24, 11, 29, 10, 29, 10, 15, 10, 21, 10, 26, 9, 26, 9)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (71, 0, 65, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 20, 14, 28, 12, 34, 11, 34, 11, 17, 11, 25, 11, 29, 10, 29, 10)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (72, 0, 80, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 22, 15, 30, 13, 37, 12, 37, 12, 18, 12, 27, 12, 32, 11, 32, 11)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (73, 0, 100, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 24, 16, 34, 14, 41, 14, 41, 14, 21, 14, 30, 13, 35, 12, 35, 12)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (74, 0, 125, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 25, 17, 36, 15, 45, 15, 45, 15, 22, 16, 33, 14, 37, 13, 37, 13)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (75, 0, 150, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 28, 19, 40, 16, 47, 16, 47, 16, 23, 16, 36, 15, 41, 14, 41, 14)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (76, 0, 200, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 35, 22, 47, 19, 61, 17, 61, 17, 28, 20, 42, 16, 50, 16, 50, 16)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (77, 0, 250, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 40, 26, 56, 22, 68, 18, 68, 18, 33, 22, 47, 18, 57, 17, 57, 17)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (78, 0, 300, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 46, 29, 64, 23, 76, 21, 76, 21, 37, 24, 52, 21, 61, 18, 61, 18)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (79, 0, 350, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 50, 32, 68, 25, 84, 22, 84, 22, 40, 27, 55, 22, 69, 19, 69, 19)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (80, 0, 400, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 56, 34, 75, 28, 91, 22, 91, 22, 43, 28, 60, 24, 74, 21, 74, 21)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (81, 0, 450, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 60, 36, 82, 28, 99, 23, 99, 23, 47, 31, 68, 27, 78, 22, 78, 22)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (82, 0, 500, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 65, 40, 92, 31, 112, 24, 112, 24, 50, 32, 72, 28, 86, 23, 86, 23)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (83, 0, 600, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 72, 42, 103, 33, 125, 26, 125, 26, 58, 36, 80, 30, 97, 27, 97, 27)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (84, 0, 700, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 78, 47, 120, 35, 135, 28, 135, 28, 66, 41, 92, 32, 110, 27, 110, 27)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (85, 0, 800, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 91, 53, 129, 39, 156, 31, 156, 31, 73, 44, 103, 33, 120, 29, 120, 29)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (86, 0, 900, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 101, 55, 140, 41, 172, 32, 172, 32, 78, 48, 110, 37, 129, 32, 129, 32)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (87, 0, 1000, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 111, 57, 146, 44, 183, 36, 183, 36, 86, 52, 121, 40, 141, 34, 141, 34)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (88, 0, 1200, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 135, 63, 188, 47, 220, 40, 220, 40, 98, 58, 136, 46, 164, 38, 164, 38)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (89, 0, 1400, 3, N'К', 50, 50, 50, 65, 90, 110, 110, 149, 66, 208, 51, 236, 42, 236, 42, 112, 60, 154, 50, 193, 41, 193, 41)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (90, 0, 25, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 10, NULL, 20, NULL, 29, NULL, 29, NULL, 9, NULL, 17, NULL, 26, NULL, 26, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (91, 0, 40, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 13, NULL, 23, NULL, 34, NULL, 34, NULL, 10, NULL, 21, NULL, 31, NULL, 31, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (92, 0, 50, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 14, NULL, 26, NULL, 38, NULL, 38, NULL, 12, NULL, 22, NULL, 33, NULL, 33, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (93, 0, 65, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 16, NULL, 29, NULL, 43, NULL, 43, NULL, 13, NULL, 25, NULL, 38, NULL, 38, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (94, 0, 80, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 18, NULL, 32, NULL, 47, NULL, 47, NULL, 15, NULL, 28, NULL, 41, NULL, 41, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (95, 0, 100, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 20, NULL, 35, NULL, 52, NULL, 52, NULL, 16, NULL, 30, NULL, 45, NULL, 45, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (96, 0, 125, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 22, NULL, 40, NULL, 57, NULL, 57, NULL, 19, NULL, 34, NULL, 49, NULL, 49, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (97, 0, 15, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 8, NULL, 16, NULL, 24, NULL, 24, NULL, 7, NULL, 14, NULL, 21, NULL, 21, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (98, 0, 20, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 9, NULL, 18, NULL, 27, NULL, 27, NULL, 8, NULL, 16, NULL, 24, NULL, 24, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (99, 0, 150, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 25, NULL, 45, NULL, 63, NULL, 63, NULL, 21, NULL, 38, NULL, 53, NULL, 53, NULL)

SET IDENTITY_INSERT [dbo].[39_normy_teplovyh_poter] OFF

GO
SET IDENTITY_INSERT [dbo].[39_normy_teplovyh_poter] ON 


INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (100, 0, 200, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 31, NULL, 54, NULL, 77, NULL, 77, NULL, 26, NULL, 46, NULL, 65, NULL, 65, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (101, 0, 250, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 36, NULL, 62, NULL, 89, NULL, 89, NULL, 30, NULL, 53, NULL, 74, NULL, 74, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (102, 0, 300, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 41, NULL, 72, NULL, 99, NULL, 99, NULL, 34, NULL, 59, NULL, 83, NULL, 83, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (103, 0, 350, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 47, NULL, 79, NULL, 109, NULL, 109, NULL, 39, NULL, 65, NULL, 91, NULL, 91, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (104, 0, 400, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 52, NULL, 86, NULL, 120, NULL, 120, NULL, 42, NULL, 72, NULL, 99, NULL, 99, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (105, 0, 450, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 57, NULL, 93, NULL, 128, NULL, 128, NULL, 46, NULL, 76, NULL, 106, NULL, 106, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (106, 0, 500, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 62, NULL, 101, NULL, 140, NULL, 140, NULL, 50, NULL, 83, NULL, 116, NULL, 116, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (107, 0, 600, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 71, NULL, 116, NULL, 159, NULL, 159, NULL, 57, NULL, 95, NULL, 131, NULL, 131, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (108, 0, 700, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 81, NULL, 130, NULL, 177, NULL, 177, NULL, 65, NULL, 105, NULL, 146, NULL, 146, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (109, 0, 800, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 91, NULL, 145, NULL, 197, NULL, 197, NULL, 72, NULL, 116, NULL, 148, NULL, 148, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (110, 0, 900, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 100, NULL, 159, NULL, 216, NULL, 216, NULL, 79, NULL, 128, NULL, 177, NULL, 177, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (111, 0, 1000, 3, N'Н', 50, 50, 50, 50, 100, 150, 150, 109, NULL, 175, NULL, 235, NULL, 235, NULL, 87, NULL, 141, NULL, 192, NULL, 192, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (112, 0, 25, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 15, 11, 20, 10, 25, 9, 25, 9, 13, 10, 18, 10, 23, 8, 23, 8)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (113, 0, 32, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 16, 12, 21, 12, 27, 10, 27, 10, 14, 11, 19, 11, 26, 8, 26, 8)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (114, 0, 40, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 17, 13, 23, 12, 30, 10, 30, 10, 15, 12, 20, 12, 26, 10, 26, 10)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (115, 0, 50, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 19, 15, 26, 14, 34, 12, 34, 12, 17, 13, 23, 12, 30, 10, 30, 10)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (116, 0, 65, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 23, 17, 31, 16, 39, 13, 39, 13, 20, 15, 27, 15, 34, 12, 34, 12)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (117, 0, 80, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 25, 19, 34, 18, 43, 14, 43, 14, 21, 16, 29, 16, 38, 13, 38, 13)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (118, 0, 100, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 28, 21, 38, 20, 48, 16, 48, 16, 24, 18, 33, 17, 42, 15, 42, 15)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (119, 0, 125, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 32, 24, 42, 23, 54, 18, 54, 18, 27, 21, 37, 20, 47, 16, 47, 16)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (120, 0, 150, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 36, 28, 48, 26, 60, 21, 60, 21, 31, 23, 41, 22, 53, 18, 53, 18)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (121, 0, 200, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 46, 34, 60, 32, 79, 22, 79, 22, 38, 28, 52, 28, 65, 21, 65, 21)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (122, 0, 250, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 54, 41, 70, 37, 94, 25, 94, 25, 45, 34, 59, 32, 78, 23, 78, 23)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (123, 0, 300, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 61, 47, 81, 43, 107, 28, 107, 28, 51, 39, 68, 36, 88, 26, 88, 26)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (124, 0, 350, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 68, 52, 90, 49, 120, 32, 120, 32, 58, 43, 75, 41, 100, 27, 100, 27)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (125, 0, 400, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 76, 58, 100, 52, 134, 33, 134, 33, 64, 48, 83, 44, 109, 31, 109, 31)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (126, 0, 450, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 84, 64, 110, 59, 148, 35, 148, 35, 70, 52, 92, 47, 119, 33, 119, 33)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (127, 0, 500, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 93, 70, 120, 64, 164, 35, 164, 35, 76, 58, 99, 52, 132, 35, 132, 35)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (128, 0, 600, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 107, 81, 141, 73, 177, 36, 177, 36, 88, 66, 116, 60, 150, 42, 150, 42)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (129, 0, 700, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 73, 53, 167, 82, 215, 45, 215, 45, 99, 78, 131, 66, 103, 25, 103, 25)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (130, 0, 800, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 139, 100, 180, 88, 243, 50, 243, 50, 111, 83, 148, 73, 124, 30, 124, 30)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (131, 0, 900, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 155, 112, 201, 99, 276, 51, 276, 51, 123, 92, 163, 81, 212, 53, 212, 53)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (132, 0, 1000, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 170, 123, 225, 111, 297, 59, 297, 59, 135, 101, 180, 88, 234, 57, 234, 57)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (133, 0, 1200, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 200, 145, 261, 129, 357, 65, 357, 65, 160, 120, 212, 104, 278, 64, 278, 64)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (134, 0, 25, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 11, 7, 16, 6, 20, 7, 20, 7, 10, 6, 15, 6, 18, 6, 18, 6)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (135, 0, 32, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 13, 8, 18, 7, 21, 7, 21, 7, 11, 7, 15, 7, 20, 6, 20, 6)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (136, 0, 40, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 13, 8, 19, 8, 23, 7, 23, 7, 11, 8, 17, 7, 21, 7, 21, 7)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (137, 0, 50, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 15, 10, 20, 9, 26, 8, 26, 8, 12, 9, 18, 8, 23, 7, 23, 7)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (138, 0, 1400, 4, N'Б', 50, 50, 50, 65, 90, 110, 110, 233, 169, 301, 149, 414, 74, 414, 74, 184, 139, 245, 120, 327, 69, 327, 69)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (139, 0, 65, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 17, 11, 24, 10, 30, 9, 30, 9, 15, 10, 21, 9, 26, 8, 26, 8)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (140, 0, 80, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 18, 12, 25, 11, 32, 9, 32, 9, 16, 11, 22, 10, 28, 9, 28, 9)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (141, 0, 100, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 20, 14, 28, 12, 35, 11, 35, 11, 17, 12, 24, 10, 30, 10, 30, 10)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (142, 0, 125, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 23, 15, 33, 13, 41, 11, 41, 11, 20, 14, 28, 12, 34, 11, 34, 11)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (143, 0, 150, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 25, 17, 37, 14, 44, 13, 44, 13, 21, 15, 30, 13, 37, 12, 37, 12)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (144, 0, 200, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 31, 21, 44, 17, 55, 15, 55, 15, 27, 18, 37, 15, 46, 14, 46, 14)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (145, 0, 250, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 37, 24, 51, 20, 64, 17, 64, 17, 31, 21, 44, 17, 53, 16, 53, 16)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (146, 0, 300, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 42, 28, 59, 22, 71, 19, 71, 19, 35, 23, 49, 19, 59, 18, 59, 18)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (147, 0, 350, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 46, 31, 66, 24, 81, 20, 81, 20, 38, 0, 54, 22, 67, 18, 67, 18)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (148, 0, 400, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 50, 34, 74, 25, 88, 22, 88, 22, 40, 30, 60, 23, 73, 20, 73, 20)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (149, 0, 450, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 56, 36, 79, 28, 97, 23, 97, 23, 47, 30, 64, 25, 79, 22, 79, 22)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (150, 0, 500, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 63, 38, 88, 30, 110, 21, 110, 21, 51, 32, 70, 27, 85, 24, 85, 24)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (151, 0, 600, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 72, 43, 101, 33, 125, 25, 125, 25, 60, 35, 81, 30, 100, 25, 100, 25)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (152, 0, 700, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 82, 48, 117, 33, 138, 29, 138, 29, 66, 40, 92, 32, 111, 27, 111, 27)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (153, 0, 800, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 93, 51, 133, 35, 155, 31, 155, 31, 74, 44, 104, 34, 122, 30, 122, 30)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (154, 0, 900, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 104, 56, 143, 43, 173, 33, 173, 33, 80, 50, 112, 39, 137, 32, 137, 32)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (155, 0, 1000, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 116, 59, 155, 46, 187, 37, 187, 37, 89, 54, 121, 44, 147, 35, 147, 35)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (156, 0, 1200, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 141, 65, 189, 49, 222, 40, 222, 40, 107, 61, 143, 50, 175, 40, 175, 40)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (157, 0, 1400, 4, N'К', 50, 50, 50, 65, 90, 110, 110, 164, 71, 219, 53, 254, 46, 254, 46, 123, 67, 166, 54, 199, 44, 199, 44)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (158, 0, 450, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 56, NULL, 94, NULL, 129, NULL, 129, NULL, 47, NULL, 78, NULL, 109, NULL, 109, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (159, 0, 500, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 61, NULL, 101, NULL, 139, NULL, 139, NULL, 50, NULL, 84, NULL, 117, NULL, 117, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (160, 0, 600, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 71, NULL, 116, NULL, 159, NULL, 159, NULL, 58, NULL, 96, NULL, 132, NULL, 132, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (161, 0, 700, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 78, NULL, 129, NULL, 175, NULL, 175, NULL, 65, NULL, 107, NULL, 146, NULL, 146, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (162, 0, 800, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 88, NULL, 143, NULL, 194, NULL, 194, NULL, 71, NULL, 118, NULL, 162, NULL, 162, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (163, 0, 900, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 96, NULL, 157, NULL, 213, NULL, 213, NULL, 78, NULL, 129, NULL, 176, NULL, 176, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (164, 0, 1000, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 106, NULL, 171, NULL, 231, NULL, 231, NULL, 86, NULL, 140, NULL, 191, NULL, 191, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (165, 0, 1400, 4, N'Н', 50, 50, 50, 50, 100, 150, 150, 142, NULL, 227, NULL, 305, NULL, 305, NULL, 114, NULL, 185, NULL, 250, NULL, 250, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (166, 32, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 25, 20, 32, 20, 38, 20, 38, 20, 25, 20, 32, 20, 38, 20, 38, 20)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (167, 38, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 26, 21, 34, 21, 40, 21, 40, 21, 26, 21, 34, 21, 40, 21, 40, 21)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (168, 45, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 28, 22, 36, 22, 42, 22, 42, 22, 28, 22, 36, 22, 42, 22, 42, 22)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (169, 57, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 31, 25, 40, 25, 47, 25, 47, 25, 31, 25, 40, 25, 47, 25, 47, 25)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (170, 76, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 35, 29, 45, 29, 53, 29, 53, 29, 35, 29, 45, 29, 53, 29, 53, 29)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (171, 89, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 38, 31, 49, 31, 57, 31, 57, 31, 38, 31, 49, 31, 57, 31, 57, 31)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (172, 108, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 42, 34, 54, 34, 62, 34, 62, 34, 42, 34, 54, 34, 62, 34, 62, 34)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (173, 133, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 45, 36, 59, 36, 68, 36, 68, 36, 45, 36, 59, 36, 68, 36, 68, 36)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (174, 159, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 52, 42, 65, 42, 75, 42, 75, 42, 52, 42, 65, 42, 75, 42, 75, 42)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (175, 194, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 56, 45, 74, 45, 83, 45, 83, 45, 56, 45, 74, 45, 83, 45, 83, 45)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (176, 219, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 62, 51, 79, 51, 91, 51, 91, 51, 62, 51, 79, 51, 91, 51, 91, 51)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (177, 273, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 72, 60, 90, 60, 103, 60, 103, 60, 72, 60, 90, 60, 103, 60, 103, 60)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (178, 325, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 81, 68, 100, 68, 115, 68, 115, 68, 81, 68, 100, 68, 115, 68, 115, 68)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (179, 377, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 88, 76, 107, 76, 126, 76, 126, 76, 90, 76, 107, 76, 126, 76, 126, 76)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (180, 426, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 98, 82, 121, 82, 137, 82, 137, 82, 99, 82, 121, 82, 137, 82, 137, 82)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (181, 480, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 107, 91, 132, 91, 150, 91, 150, 91, 110, 91, 132, 91, 150, 91, 150, 91)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (182, 530, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 115, 101, 142, 101, 160, 101, 160, 101, 120, 101, 142, 101, 160, 101, 160, 101)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (183, 630, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 132, 114, 163, 114, 184, 114, 184, 114, 138, 114, 163, 114, 184, 114, 184, 114)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (184, 720, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 147, 125, 181, 125, 202, 125, 202, 125, 155, 125, 181, 125, 202, 125, 202, 125)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (185, 820, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 163, 141, 200, 141, 223, 141, 223, 141, 174, 141, 200, 141, 223, 141, 223, 141)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (186, 920, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 178, 155, 218, 155, 244, 155, 244, 155, 193, 155, 218, 155, 244, 155, 244, 155)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (187, 1020, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 196, 170, 240, 170, 266, 170, 266, 170, 212, 170, 240, 170, 266, 170, 266, 170)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (188, 1220, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 229, 200, 282, 200, 308, 200, 308, 200, 229, 200, 282, 200, 308, 200, 308, 200)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (189, 1420, 0, 1, N'Б', 5, 5, 50, 65, 90, 110, 110, 260, 228, 326, 228, 352, 228, 352, 228, 260, 228, 326, 228, 352, 228, 352, 228)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (190, 18, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 11, NULL, 19, NULL, 27, NULL, 33, NULL, 11, NULL, 19, NULL, 27, NULL, 33, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (191, 25, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 13, NULL, 21, NULL, 29, NULL, 36, NULL, 13, NULL, 21, NULL, 29, NULL, 36, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (192, 32, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 15, NULL, 23, NULL, 31, NULL, 38, NULL, 15, NULL, 23, NULL, 31, NULL, 38, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (193, 38, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 16, NULL, 24, NULL, 33, NULL, 41, NULL, 16, NULL, 24, NULL, 33, NULL, 41, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (194, 45, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 18, NULL, 27, NULL, 36, NULL, 45, NULL, 18, NULL, 27, NULL, 36, NULL, 45, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (195, 57, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 21, NULL, 30, NULL, 40, NULL, 49, NULL, 21, NULL, 30, NULL, 40, NULL, 49, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (196, 76, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 25, NULL, 35, NULL, 45, NULL, 55, NULL, 25, NULL, 35, NULL, 45, NULL, 55, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (197, 89, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 28, NULL, 38, NULL, 50, NULL, 60, NULL, 28, NULL, 38, NULL, 50, NULL, 60, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (198, 108, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 31, NULL, 43, NULL, 55, NULL, 67, NULL, 31, NULL, 43, NULL, 55, NULL, 67, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (199, 133, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 35, NULL, 48, NULL, 60, NULL, 74, NULL, 35, NULL, 48, NULL, 60, NULL, 74, NULL)

SET IDENTITY_INSERT [dbo].[39_normy_teplovyh_poter] OFF

GO

SET IDENTITY_INSERT [dbo].[39_normy_teplovyh_poter] ON 

INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (200, 159, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 38, NULL, 50, NULL, 65, NULL, 80, NULL, 38, NULL, 50, NULL, 65, NULL, 80, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (201, 194, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 42, NULL, 58, NULL, 73, NULL, 88, NULL, 42, NULL, 58, NULL, 73, NULL, 88, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (202, 219, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 46, NULL, 60, NULL, 78, NULL, 95, NULL, 46, NULL, 60, NULL, 78, NULL, 95, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (203, 273, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 53, NULL, 70, NULL, 87, NULL, 107, NULL, 53, NULL, 70, NULL, 87, NULL, 107, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (204, 325, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 60, NULL, 80, NULL, 100, NULL, 120, NULL, 60, NULL, 80, NULL, 100, NULL, 120, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (205, 377, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 71, NULL, 93, NULL, 114, NULL, 135, NULL, 71, NULL, 93, NULL, 114, NULL, 135, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (206, 426, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 82, NULL, 105, NULL, 128, NULL, 150, NULL, 82, NULL, 105, NULL, 128, NULL, 150, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (207, 480, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 89, NULL, 113, NULL, 136, NULL, 160, NULL, 89, NULL, 113, NULL, 136, NULL, 160, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (208, 530, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 95, NULL, 120, NULL, 145, NULL, 170, NULL, 95, NULL, 120, NULL, 145, NULL, 170, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (209, 630, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 104, NULL, 133, NULL, 160, NULL, 190, NULL, 104, NULL, 133, NULL, 160, NULL, 190, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (210, 720, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 115, NULL, 145, NULL, 176, NULL, 206, NULL, 115, NULL, 145, NULL, 176, NULL, 206, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (211, 820, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 135, NULL, 168, NULL, 200, NULL, 233, NULL, 135, NULL, 168, NULL, 200, NULL, 233, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (212, 920, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 155, NULL, 190, NULL, 225, NULL, 260, NULL, 155, NULL, 190, NULL, 225, NULL, 260, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (213, 1020, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 180, NULL, 220, NULL, 255, NULL, 292, NULL, 180, NULL, 220, NULL, 255, NULL, 292, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (214, 1220, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 201, NULL, 246, NULL, 286, NULL, 333, NULL, 201, NULL, 246, NULL, 286, NULL, 333, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (215, 1420, 0, 1, N'Н', 5, 5, 50, 50, 75, 100, 125, 230, NULL, 280, NULL, 325, NULL, 380, NULL, 230, NULL, 280, NULL, 325, NULL, 380, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (216, 0, 25, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 15, 10, 22, 9, 27, 9, 27, 9, 14, 9, 20, 9, 24, 8, 24, 8)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (217, 0, 30, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 16, 11, 23, 10, 28, 9, 28, 9, 15, 10, 21, 9, 26, 9, 26, 9)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (218, 0, 40, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 18, 12, 25, 11, 31, 10, 31, 10, 15, 11, 22, 10, 28, 9, 28, 9)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (219, 0, 50, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 19, 13, 28, 12, 34, 11, 34, 11, 17, 12, 24, 11, 30, 10, 30, 10)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (220, 0, 65, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 23, 16, 33, 14, 40, 12, 40, 12, 20, 14, 29, 13, 34, 11, 34, 11)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (221, 0, 80, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 25, 17, 35, 15, 44, 13, 44, 13, 22, 15, 31, 14, 38, 12, 38, 12)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (222, 0, 100, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 28, 19, 40, 16, 49, 15, 49, 15, 24, 16, 35, 15, 41, 13, 41, 13)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (223, 0, 125, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 29, 20, 42, 17, 53, 15, 53, 15, 27, 18, 36, 15, 43, 14, 43, 14)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (224, 0, 150, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 33, 22, 46, 19, 56, 16, 56, 16, 28, 19, 38, 16, 47, 15, 47, 15)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (225, 0, 175, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 37, 25, 52, 21, 64, 18, 64, 18, 31, 22, 42, 18, 53, 16, 53, 16)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (226, 0, 200, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 41, 27, 57, 22, 71, 20, 71, 20, 34, 23, 46, 19, 59, 18, 59, 18)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (227, 0, 250, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 46, 30, 65, 25, 80, 22, 80, 22, 39, 26, 55, 22, 66, 20, 66, 20)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (228, 0, 300, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 53, 34, 75, 28, 89, 24, 89, 24, 43, 28, 60, 24, 72, 22, 72, 22)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (229, 0, 350, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 59, 38, 80, 29, 101, 25, 101, 25, 47, 32, 65, 26, 81, 22, 81, 22)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (230, 0, 400, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 65, 40, 94, 32, 106, 26, 106, 26, 50, 33, 71, 28, 87, 24, 87, 24)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (231, 0, 450, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 66, 42, 96, 34, 116, 28, 116, 28, 58, 37, 80, 31, 92, 25, 92, 25)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (232, 0, 500, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 76, 46, 108, 37, 144, 28, 144, 28, 59, 38, 84, 33, 101, 28, 101, 28)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (233, 0, 600, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 84, 50, 121, 39, 147, 30, 147, 30, 68, 43, 94, 35, 114, 29, 114, 29)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (234, 0, 700, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 92, 54, 140, 40, 159, 33, 159, 33, 77, 47, 108, 37, 130, 32, 130, 32)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (235, 0, 800, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 112, 62, 156, 41, 183, 36, 183, 36, 86, 52, 121, 39, 140, 34, 140, 34)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (236, 0, 900, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 119, 65, 164, 49, 201, 38, 201, 38, 91, 57, 130, 46, 160, 37, 160, 37)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (237, 0, 1000, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 131, 67, 171, 51, 214, 42, 214, 42, 101, 61, 136, 49, 165, 40, 165, 40)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (238, 0, 1200, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 159, 74, 221, 57, 258, 46, 258, 46, 124, 68, 159, 55, 197, 45, 197, 45)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (239, 0, 1400, 2, N'К', 50, 50, 50, 65, 90, 110, 110, 176, 77, 245, 59, 277, 50, 277, 50, 131, 71, 181, 59, 217, 48, 217, 48)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (240, 0, 25, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 31, 23, 41, 22, 41, 22, 41, 22, 28, 22, 38, 21, 38, 21, 38, 21)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (241, 0, 30, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 32, 24, 43, 23, 43, 23, 43, 23, 29, 22, 40, 22, 40, 22, 40, 22)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (242, 0, 40, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 33, 27, 47, 26, 47, 26, 47, 26, 32, 25, 43, 23, 43, 23, 43, 23)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (243, 0, 50, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 38, 29, 52, 28, 52, 28, 52, 28, 34, 27, 46, 25, 46, 25, 46, 25)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (244, 0, 65, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 43, 33, 58, 31, 58, 31, 58, 31, 39, 29, 52, 28, 52, 28, 52, 28)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (245, 0, 80, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 44, 34, 59, 32, 59, 32, 59, 32, 40, 30, 53, 29, 53, 29, 53, 29)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (246, 0, 100, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 47, 36, 64, 34, 64, 34, 64, 34, 42, 33, 56, 30, 56, 30, 56, 30)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (247, 0, 125, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 53, 40, 70, 38, 70, 38, 70, 38, 46, 35, 62, 34, 62, 34, 62, 34)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (248, 0, 150, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 59, 45, 78, 42, 78, 42, 78, 42, 52, 40, 69, 37, 69, 37, 69, 37)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (249, 0, 175, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 63, 48, 83, 43, 83, 43, 83, 43, 54, 41, 73, 40, 73, 40, 73, 40)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (250, 0, 200, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 66, 51, 87, 46, 87, 46, 87, 46, 57, 43, 77, 41, 77, 41, 77, 41)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (251, 0, 250, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 71, 54, 96, 51, 96, 51, 96, 51, 62, 47, 83, 44, 83, 44, 83, 44)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (252, 0, 300, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 78, 59, 105, 55, 105, 55, 105, 55, 68, 51, 90, 48, 90, 48, 90, 48)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (253, 0, 350, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 87, 65, 115, 59, 115, 59, 115, 59, 74, 56, 97, 52, 97, 52, 97, 52)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (254, 0, 400, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 93, 69, 121, 63, 121, 63, 121, 63, 78, 59, 104, 54, 104, 54, 104, 54)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (255, 0, 450, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 100, 74, 130, 67, 130, 67, 130, 67, 84, 62, 111, 58, 111, 58, 111, 58)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (256, 0, 500, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 106, 78, 140, 71, 140, 71, 140, 71, 90, 67, 119, 62, 119, 62, 119, 62)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (257, 0, 600, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 121, 89, 160, 81, 160, 81, 160, 81, 101, 75, 134, 69, 134, 69, 134, 69)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (258, 0, 700, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 134, 96, 175, 86, 175, 86, 175, 86, 108, 80, 146, 74, 146, 74, 146, 74)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (259, 0, 800, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 146, 105, 195, 94, 195, 94, 195, 94, 121, 88, 160, 80, 160, 80, 160, 80)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (260, 0, 900, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 157, 114, 214, 101, 214, 101, 214, 101, 132, 96, 174, 86, 174, 86, 174, 86)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (261, 0, 1000, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 168, 122, 234, 109, 234, 109, 234, 109, 145, 103, 187, 92, 187, 92, 187, 92)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (262, 0, 1200, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 190, 139, 273, 125, 273, 125, 273, 125, 169, 119, 215, 104, 215, 104, 215, 104)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (263, 0, 15, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 10, NULL, 19, NULL, 29, NULL, 29, NULL, 9, NULL, 17, NULL, 26, NULL, 26, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (264, 0, 20, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 11, NULL, 22, NULL, 33, NULL, 33, NULL, 10, NULL, 19, NULL, 29, NULL, 29, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (265, 0, 25, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 13, NULL, 24, NULL, 36, NULL, 36, NULL, 11, NULL, 22, NULL, 32, NULL, 32, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (266, 0, 40, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 15, NULL, 28, NULL, 42, NULL, 42, NULL, 13, NULL, 25, NULL, 38, NULL, 38, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (267, 0, 50, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 16, NULL, 31, NULL, 46, NULL, 46, NULL, 15, NULL, 27, NULL, 40, NULL, 40, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (268, 0, 65, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 20, NULL, 35, NULL, 53, NULL, 53, NULL, 16, NULL, 31, NULL, 46, NULL, 46, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (269, 0, 80, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 22, NULL, 39, NULL, 57, NULL, 57, NULL, 18, NULL, 34, NULL, 50, NULL, 50, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (270, 0, 100, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 21, NULL, 43, NULL, 63, NULL, 63, NULL, 23, NULL, 37, NULL, 55, NULL, 55, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (271, 0, 125, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 28, NULL, 48, NULL, 70, NULL, 70, NULL, 24, NULL, 42, NULL, 60, NULL, 60, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (272, 0, 150, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 30, NULL, 54, NULL, 77, NULL, 77, NULL, 26, NULL, 46, NULL, 66, NULL, 66, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (273, 0, 175, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 34, NULL, 60, NULL, 85, NULL, 85, NULL, 29, NULL, 52, NULL, 71, NULL, 71, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (274, 0, 200, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 38, NULL, 66, NULL, 94, NULL, 94, NULL, 32, NULL, 56, NULL, 80, NULL, 80, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (275, 0, 250, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 44, NULL, 76, NULL, 108, NULL, 108, NULL, 37, NULL, 65, NULL, 91, NULL, 91, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (276, 0, 300, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 51, NULL, 87, NULL, 121, NULL, 121, NULL, 42, NULL, 72, NULL, 102, NULL, 102, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (277, 0, 1400, 2, N'Б', 50, 50, 50, 65, 90, 90, 90, 212, 157, 313, 169, 313, 169, 313, 169, 193, 134, 243, 116, 243, 116, 243, 116)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (278, 0, 350, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 57, NULL, 96, NULL, 133, NULL, 133, NULL, 47, NULL, 80, NULL, 113, NULL, 113, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (279, 0, 400, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 63, NULL, 105, NULL, 146, NULL, 146, NULL, 53, NULL, 88, NULL, 122, NULL, 122, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (280, 0, 450, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 69, NULL, 114, NULL, 157, NULL, 157, NULL, 56, NULL, 94, NULL, 131, NULL, 131, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (281, 0, 500, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 76, NULL, 123, NULL, 170, NULL, 170, NULL, 61, NULL, 102, NULL, 143, NULL, 143, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (282, 0, 600, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 86, NULL, 142, NULL, 194, NULL, 194, NULL, 71, NULL, 117, NULL, 162, NULL, 162, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (283, 0, 700, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 98, NULL, 158, NULL, 215, NULL, 215, NULL, 79, NULL, 130, NULL, 180, NULL, 180, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (284, 0, 800, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 110, NULL, 177, NULL, 239, NULL, 239, NULL, 89, NULL, 144, NULL, 183, NULL, 183, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (285, 0, 900, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 121, NULL, 195, NULL, 263, NULL, 263, NULL, 97, NULL, 158, NULL, 218, NULL, 218, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (286, 0, 1000, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 133, NULL, 213, NULL, 287, NULL, 287, NULL, 107, NULL, 173, NULL, 287, NULL, 287, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (287, 0, 1200, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 157, NULL, 249, NULL, 333, NULL, 333, NULL, 126, NULL, 202, NULL, 274, NULL, 274, NULL)
INSERT [dbo].[39_normy_teplovyh_poter] ([id], [d], [dy], [date], [proklad], [tg], [tn], [t2], [t1_1], [t1_2], [t1_3], [t1_4], [qp_1], [qo_1], [qp_2], [qo_2], [qp_3], [qo_3], [qp_4], [qo_4], [qp_1gt5000], [qo_1gt5000], [qp_2gt5000], [qo_2gt5000], [qp_3gt5000], [qo_3gt5000], [qp_4gt5000], [qo_4gt5000]) VALUES (288, 0, 1400, 2, N'Н', 50, 50, 50, 50, 100, 150, 150, 181, NULL, 285, NULL, 379, NULL, 379, NULL, 145, NULL, 231, NULL, 312, NULL, 312, NULL)
SET IDENTITY_INSERT [dbo].[39_normy_teplovyh_poter] OFF
GO
SET IDENTITY_INSERT [dbo].[40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy] ON 

INSERT [dbo].[40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy] ([id], [t_r], [alfa]) VALUES (1, 0, 2.05)
INSERT [dbo].[40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy] ([id], [t_r], [alfa]) VALUES (2, -5, 1.67)
INSERT [dbo].[40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy] ([id], [t_r], [alfa]) VALUES (3, -10, 1.45)
INSERT [dbo].[40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy] ([id], [t_r], [alfa]) VALUES (4, -15, 1.29)
INSERT [dbo].[40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy] ([id], [t_r], [alfa]) VALUES (5, -20, 1.17)
INSERT [dbo].[40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy] ([id], [t_r], [alfa]) VALUES (6, -25, 1.08)
INSERT [dbo].[40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy] ([id], [t_r], [alfa]) VALUES (7, -30, 1)
INSERT [dbo].[40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy] ([id], [t_r], [alfa]) VALUES (8, -35, 0.95)
INSERT [dbo].[40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy] ([id], [t_r], [alfa]) VALUES (9, -40, 0.9)
INSERT [dbo].[40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy] ([id], [t_r], [alfa]) VALUES (10, -45, 0.85)
INSERT [dbo].[40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy] ([id], [t_r], [alfa]) VALUES (11, -50, 0.82)
INSERT [dbo].[40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy] ([id], [t_r], [alfa]) VALUES (12, -55, 0.8)
SET IDENTITY_INSERT [dbo].[40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy] OFF
GO
SET IDENTITY_INSERT [dbo].[41_otsenka_faktorov_otritsatelnyh_vozdeystviy_na_truboprovod] ON 

INSERT [dbo].[41_otsenka_faktorov_otritsatelnyh_vozdeystviy_na_truboprovod] ([id], [nomer], [date], [f_kol_let_lt7], [f_kol_let_lt15], [f_kol_let_lt20], [f_kol_let_lt25], [f_kol_let_gt25], [f_koef_povrlt1], [f_koef_povrlt2], [f_koef_povrlt4], [f_koef_povrlt5], [f_koef_povrgt5], [f_koef_povr_2godalt1], [f_koef_povr_2godalt2], [f_koef_povr_2godalt4], [f_koef_povr_2godalt5], [f_koef_povr_2godagt5], [f_10_elektro], [f_5_elektro], [f_3_elektro], [f_1_elektro], [f_0_elektro], [f_peres_elektro], [f_10_gas], [f_5_gas], [f_3_gas], [f_1_gas], [f_0_gas], [f_peres_gas], [f_10_transp], [f_5_transp], [f_3_transp], [f_1_transp], [f_0_transp], [f_peres_transp], [f_elektropotenzlt0], [f_elektropotenzlt04], [f_elektropotenzgt04], [f_nar_korroz], [f_vnu_korroz], [f_grund_water], [f_pavod_water], [f_vod_water], [f_fek_water], [f_st_stik], [f_st_islom], [f_st_otkl], [f_st_nes], [f_st_opor], [f_water_otved], [f_otkl], [f_okl_pot], [f_slog_arbeit], [f_tratuar], [f_doroga], [f_ucherb_narod], [f_ucherb_gorod]) VALUES (1, 1, NULL, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 0, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
SET IDENTITY_INSERT [dbo].[41_otsenka_faktorov_otritsatelnyh_vozdeystviy_na_truboprovod] OFF
GO
SET IDENTITY_INSERT [dbo].[42_koeffitsienty_opredeleniya_tehnologicheskih_psv] ON 

INSERT [dbo].[42_koeffitsienty_opredeleniya_tehnologicheskih_psv] ([id], [kod], [name_psv], [kratnost_psv], [help]) VALUES (1, 1, N'Пусковые ПСВ', 1.5, N'Нормативные пусковые ПСВ будут определяться из 1.5-кратного объема ТС и систем теплопотребления.РД 153-34.0-20.523-98.')
INSERT [dbo].[42_koeffitsienty_opredeleniya_tehnologicheskih_psv] ([id], [kod], [name_psv], [kratnost_psv], [help]) VALUES (2, 2, N'ПСВ испытаний', 0.5, N'Нормативные ПСВ испытаний будут определяться из 0.5-кратного объема ТС и систем теплопотребления.РД 153-34.0-20.523-98.')
SET IDENTITY_INSERT [dbo].[42_koeffitsienty_opredeleniya_tehnologicheskih_psv] OFF
GO
SET IDENTITY_INSERT [dbo].[43_teplovaya_akkumulyatsiya_zdaniy] ON 

INSERT [dbo].[43_teplovaya_akkumulyatsiya_zdaniy] ([id], [nomer], [harakter], [serija], [utepl], [tol_stena], [tol_izol], [help], [pomechenie], [k_akkuml]) VALUES (1, 1, N'Крупнопанельный дом', N'1-605 А', N'Минеральные плиты с железнобетонными фактурными слоями', N'21 см', N'в том числе утеплителя 12 см', N'с трехслойными наружными стенами', N'Угловые , верхний, средние этажи', 42)
INSERT [dbo].[43_teplovaya_akkumulyatsiya_zdaniy] ([id], [nomer], [harakter], [serija], [utepl], [tol_stena], [tol_izol], [help], [pomechenie], [k_akkuml]) VALUES (2, 2, N'Крупнопанельный дом стена', NULL, N'Минеральные плиты с железнобетонными фактурными слоями', N'21 см', N'в том числе утеплителя 12 см', N'с трехслойными наружными стенами', N'Угловые,     первый этаж', 46)
INSERT [dbo].[43_teplovaya_akkumulyatsiya_zdaniy] ([id], [nomer], [harakter], [serija], [utepl], [tol_stena], [tol_izol], [help], [pomechenie], [k_akkuml]) VALUES (3, 3, N'Крупнопанельный дом  жилой дом', N'К7-3 (конструкция инженера Лагуненко)', N'Минеральные плиты с железнобетонными фактурными слоями', N'16 см', NULL, NULL, N'Угловые, верхний этаж', 32)
INSERT [dbo].[43_teplovaya_akkumulyatsiya_zdaniy] ([id], [nomer], [harakter], [serija], [utepl], [tol_stena], [tol_izol], [help], [pomechenie], [k_akkuml]) VALUES (4, 4, N'Крупнопанельный дом  жилой дом', N'К7-3 (конструкция инженера Лагуненко)', N'Минеральные плиты с железнобетонными фактурными слоями', N'16 см', NULL, NULL, N'Угловые, средний этаж', 40)
INSERT [dbo].[43_teplovaya_akkumulyatsiya_zdaniy] ([id], [nomer], [harakter], [serija], [utepl], [tol_stena], [tol_izol], [help], [pomechenie], [k_akkuml]) VALUES (5, 5, N'Крупнопанельный дом  жилой дом', N'К7-3 (конструкция инженера Лагуненко)', N'Минеральные плиты с железнобетонными фактурными слоями', N'16 см', NULL, NULL, N'Средний этаж', 51)
INSERT [dbo].[43_teplovaya_akkumulyatsiya_zdaniy] ([id], [nomer], [harakter], [serija], [utepl], [tol_stena], [tol_izol], [help], [pomechenie], [k_akkuml]) VALUES (6, 6, N'Дом из объемных материалов', NULL, N'Наружные ограждения из железнобетонных элементов, утепленных минераловатными плитами', N'22 см', N'в зоне стыкования с ребрами 5 см, между ребрами - 7 см', N'Толщина железнобетонных элементов между ребрами 30', N'Угловые верхнего этажа', 40)
INSERT [dbo].[43_teplovaya_akkumulyatsiya_zdaniy] ([id], [nomer], [harakter], [serija], [utepl], [tol_stena], [tol_izol], [help], [pomechenie], [k_akkuml]) VALUES (7, 7, N'Кирпичные жилые здания', NULL, NULL, N'2.5 кирпича', NULL, N'Коэффициент остекления 0.18-0.25', N'Угловые', 60)
INSERT [dbo].[43_teplovaya_akkumulyatsiya_zdaniy] ([id], [nomer], [harakter], [serija], [utepl], [tol_stena], [tol_izol], [help], [pomechenie], [k_akkuml]) VALUES (8, 8, N'Кирпичные жилые здания', NULL, NULL, N'2.5 кирпича', NULL, N'Коэффициент остекления 0.18-0.25', N'Угловые', 65)
INSERT [dbo].[43_teplovaya_akkumulyatsiya_zdaniy] ([id], [nomer], [harakter], [serija], [utepl], [tol_stena], [tol_izol], [help], [pomechenie], [k_akkuml]) VALUES (9, 9, N'Кирпичные жилые здания', NULL, NULL, N'2.5 кирпича', NULL, N'Коэффициент остекления 0.18-0.25', N'Средние', 66)
INSERT [dbo].[43_teplovaya_akkumulyatsiya_zdaniy] ([id], [nomer], [harakter], [serija], [utepl], [tol_stena], [tol_izol], [help], [pomechenie], [k_akkuml]) VALUES (10, 10, N'Кирпичные жилые здания', NULL, NULL, N'2.5 кирпича', NULL, N'Коэффициент остекления 0.18-0.25', N'Угловые', 100)
INSERT [dbo].[43_teplovaya_akkumulyatsiya_zdaniy] ([id], [nomer], [harakter], [serija], [utepl], [tol_stena], [tol_izol], [help], [pomechenie], [k_akkuml]) VALUES (11, 11, N'Промышленные здания', NULL, NULL, N'2 кирпича', NULL, N'Незначительные внутренние выделения. Коэффициент остекления 0.15-0.3', NULL, 14)
INSERT [dbo].[43_teplovaya_akkumulyatsiya_zdaniy] ([id], [nomer], [harakter], [serija], [utepl], [tol_stena], [tol_izol], [help], [pomechenie], [k_akkuml]) VALUES (12, 12, N'Промышленные здания', NULL, NULL, N'2 кирпича', NULL, N'Незначительные внутренние выделения. Коэффициент остекления 0.15-0.3', NULL, 25)
SET IDENTITY_INSERT [dbo].[43_teplovaya_akkumulyatsiya_zdaniy] OFF
GO
SET IDENTITY_INSERT [dbo].[44_temp_padeniya_temperatury_vnutri_zdaniy] ON 

INSERT [dbo].[44_temp_padeniya_temperatury_vnutri_zdaniy] ([id], [k_akkuml], [t_0], [t_10], [t_20], [t_30]) VALUES (1, 20, 0.8, 1.4, 1.8, 2.4)
INSERT [dbo].[44_temp_padeniya_temperatury_vnutri_zdaniy] ([id], [k_akkuml], [t_0], [t_10], [t_20], [t_30]) VALUES (2, 40, 0.5, 0.8, 1.1, 1.5)
INSERT [dbo].[44_temp_padeniya_temperatury_vnutri_zdaniy] ([id], [k_akkuml], [t_0], [t_10], [t_20], [t_30]) VALUES (3, 60, 0.4, 0.6, 0.8, 1)
SET IDENTITY_INSERT [dbo].[44_temp_padeniya_temperatury_vnutri_zdaniy] OFF
GO
SET IDENTITY_INSERT [dbo].[45_koeffitsient_nachala_kavitatsii] ON 

INSERT [dbo].[45_koeffitsient_nachala_kavitatsii] ([id], [nomer], [po], [k_kavit], [primech]) VALUES (1, 1, N'Шиберный', 0.65, NULL)
INSERT [dbo].[45_koeffitsient_nachala_kavitatsii] ([id], [nomer], [po], [k_kavit], [primech]) VALUES (2, 2, N'Односедельный клапан', 0.6, NULL)
INSERT [dbo].[45_koeffitsient_nachala_kavitatsii] ([id], [nomer], [po], [k_kavit], [primech]) VALUES (3, 3, N'Двухседельный клапан', 0.51, NULL)
INSERT [dbo].[45_koeffitsient_nachala_kavitatsii] ([id], [nomer], [po], [k_kavit], [primech]) VALUES (4, 4, N'Шаровой', 0.68, NULL)
INSERT [dbo].[45_koeffitsient_nachala_kavitatsii] ([id], [nomer], [po], [k_kavit], [primech]) VALUES (5, 5, N'Заслоночный', 0.38, N'При угле поворота 60 Град.')
SET IDENTITY_INSERT [dbo].[45_koeffitsient_nachala_kavitatsii] OFF
GO
SET IDENTITY_INSERT [dbo].[46_kvartaly_raboty_masterov] ON 

INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (1, 1, 2, N'86')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (2, 2, 2, N'104')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (3, 3, 2, N'89а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (4, 4, 2, N'72')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (5, 5, 2, N'111-112')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (6, 6, 2, N'137')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (7, 7, 2, N'139')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (8, 8, 10, N'105-109а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (9, 9, 10, N'138')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (10, 10, 10, N'138б')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (11, 11, 10, N'138а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (12, 12, 10, N'138г')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (13, 13, 10, N'108')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (14, 14, 10, N'105а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (15, 15, 10, N'107')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (16, 16, 10, N'106')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (17, 17, 10, N'69')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (18, 18, 10, N'67-68')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (19, 19, 10, N'66')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (20, 20, 10, N'51')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (21, 21, 10, N'32')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (22, 22, 10, N'31')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (23, 23, 10, N'50')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (24, 24, 10, N'31а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (25, 25, 10, N'50')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (26, 26, 5, N'144')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (27, 27, 5, N'157')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (28, 28, 5, N'168-168а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (29, 29, 5, N'167а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (30, 30, 5, N'158')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (31, 31, 3, N'132')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (32, 32, 3, N'124')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (33, 33, 3, N'123')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (34, 34, 3, N'98')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (35, 35, 3, N'93')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (36, 36, 3, N'83')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (37, 37, 3, N'92')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (38, 38, 3, N'99')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (39, 39, 3, N'122')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (40, 40, 3, N'121')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (41, 41, 3, N'133')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (42, 42, 5, N'142')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (43, 43, 5, N'159')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (44, 44, 5, N'159а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (45, 45, 5, N'166а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (46, 46, 5, N'166б,в')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (47, 47, 5, N'166')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (48, 48, 5, N'141')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (49, 49, 5, N'160')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (50, 50, 5, N'160а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (51, 51, 3, N'134')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (52, 52, 3, N'120')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (53, 53, 3, N'119')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (54, 54, 3, N'100')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (55, 55, 3, N'91')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (56, 56, 4, N'210')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (57, 57, 4, N'218')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (58, 58, 4, N'215')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (59, 59, 8, N'217')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (60, 60, 8, N'216')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (61, 61, 8, N'208')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (62, 62, 8, N'205')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (63, 63, 8, N'195')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (64, 64, 8, N'200')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (65, 65, 8, N'199')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (66, 66, 8, N'196')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (67, 67, 8, N'197')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (68, 68, 8, N'198')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (69, 69, 8, N'189')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (70, 70, 5, N'179')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (71, 71, 5, N'179а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (72, 72, 5, N'190')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (73, 73, 5, N'191')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (74, 74, 8, N'187')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (75, 75, 8, N'188')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (76, 76, 8, N'206')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (77, 77, 8, N'207')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (78, 78, 8, N'12 район')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (79, 79, 8, N'группа Б')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (80, 80, 7, N'11 р-он группа В')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (81, 81, 7, N'мкр-н Аль-Фараби ч.1 группа Д')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (82, 82, 7, N'мкр-н Аль-Фараби ч.2')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (83, 83, 3, N'130')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (84, 84, 4, N'145')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (85, 85, 3, N'129')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (86, 86, 3, N'мкр-н Самал')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (87, 87, 4, N'147')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (88, 88, 4, N'154')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (89, 89, 4, N'146')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (90, 90, 4, N'155')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (91, 91, 4, N'156')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (92, 92, 4, N'177')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (93, 93, 4, N'мкр-н Молодёжный')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (94, 94, 4, N'пос.Энергетиков')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (95, 95, 4, N'мкр-н Алатау')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (96, 96, 4, N'193')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (97, 97, 4, N'176')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (98, 98, 4, N'201')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (99, 99, 8, N'194')
GO
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (100, 100, 4, N'192')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (101, 101, 3, N'128')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (102, 102, 3, N'127')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (103, 103, 3, N'97а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (104, 104, 3, N'97')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (105, 105, 3, N'126')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (106, 106, 3, N'125')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (107, 107, 3, N'127')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (108, 108, 3, N'131')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (109, 109, 6, N'группа А')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (110, 110, 6, N'группа Г')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (111, 111, 6, N'мкр-н 5')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (112, 112, 6, N'мкр-н 4')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (113, 113, 6, N'мкр-н 2')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (114, 114, 6, N'мкр-н 1')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (115, 115, 6, N'мкр-н 2а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (116, 116, 6, N'мкр-н 3')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (117, 117, 5, N'165')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (118, 118, 5, N'161а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (119, 119, 5, N'160')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (120, 120, 5, N'140')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (121, 121, 2, N'140')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (122, 122, 2, N'135')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (123, 123, 2, N'118')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (124, 124, 2, N'117')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (125, 125, 2, N'139а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (126, 126, 2, N'136')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (127, 127, 2, N'113-116')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (128, 128, 2, N'114-115')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (129, 129, 2, N'101')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (130, 130, 2, N'102-103')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (131, 131, 2, N'90')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (132, 132, 2, N'86а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (133, 133, 2, N'90а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (134, 134, 2, N'Агрогородок')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (135, 135, 1, N'9 р-он')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (136, 136, 1, N'2 р-он')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (137, 137, 4, N'169-169а')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (138, 138, 8, N'207')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (139, 139, 10, N'109')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (140, 140, 10, N'110')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (141, 141, 2, N'87')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (142, 142, 7, N'мкр-н 9')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (143, 143, 4, N'мкр-н Целинный')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (144, 144, 1, N'ВРЗ')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (145, 145, 1, N'8 р-он')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (146, 146, 1, N'7 р-он')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (147, 147, 5, N'16 р-он')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (148, 148, 1, N'6 р-он')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (149, 149, 1, N'5 р-он')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (150, 150, 1, N'3 р-он')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (151, 151, 1, N'4 р-он')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (152, 152, 1, N'10 р-он')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (153, 153, 1, N'17 р-он')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (154, 154, 6, N'Текстильщиков')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (155, 155, 4, N'Ак-булак-1')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (156, 156, 4, N'Ак-булак-2')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (157, 157, 4, N'Ак-булак-3')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (158, 158, 4, N'Ак-булак-4')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (159, 159, 4, N'Ак-булак-5')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (160, 160, 4, N'203')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (161, 161, 4, N'214')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (162, 162, 4, N'204')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (163, 163, 4, N'209')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (164, 164, 5, N'143')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (165, 165, 9, N'Левый берег')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (166, 166, 9, N'a1')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (167, 167, 8, N'a2')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (168, 168, NULL, N'a3')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (169, 169, 3, N'a4')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (170, 170, 4, N'a5')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (171, 171, 4, N'a6')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (172, 172, 4, N'a7')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (173, 173, 4, N'a8')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (174, 174, 4, N'a9')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (175, 175, 3, N'р-н слободки ')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (176, 176, 3, N'a10')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (177, 177, 2, N'Соц. желье')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (178, 178, 8, N'13 р-он')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (179, 179, 7, N'a11')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (180, 180, 6, N'a12')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (181, 181, 6, N'мкр-н 6')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (182, 182, 6, N'VIP')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (183, 183, 6, N'a13')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (184, 184, 6, N'a14')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (185, 185, 10, N'a15')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (186, 186, 5, N'167')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (187, 187, 5, N'a16')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (188, 188, 5, N'a17')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (189, 189, 5, N'a18')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (190, 190, 5, N'a19')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (191, 191, 5, N'a20')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (192, 192, 5, N'a21')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (193, 193, 5, N'a22')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (194, 194, 5, N'a23')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (195, 195, 5, N'a24')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (196, 196, 5, N'a25')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (197, 197, 5, N'a26')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (198, 198, 5, N'a27')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (199, 199, 3, N'a28')
GO
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (200, 200, 3, N'a29')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (201, 201, 3, N'a30')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (202, 202, 3, N'145')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (203, 203, 3, N'144')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (204, 204, 3, N'143')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (205, 205, 3, N'141')
INSERT [dbo].[46_kvartaly_raboty_masterov] ([id], [nomer], [master], [name_kv]) VALUES (206, 206, 3, N'142')
SET IDENTITY_INSERT [dbo].[46_kvartaly_raboty_masterov] OFF
SET IDENTITY_INSERT [dbo].[edinitsy_aleksey] ON 

INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (1, N'%')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (2, N'бар')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (3, N'Вт/(м*К)')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (4, N'Вт/(м^2*К)')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (5, N'Вт/(м^3*К)')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (6, N'Вт/м')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (7, N'ГДж')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (8, N'Гкал')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (9, N'Гкал/ч')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (10, N'год')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (11, N'Град.С')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (12, N'Дж/(кг*К)')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (13, N'Дж/К')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (14, N'дня(ей)')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (15, N'кВ')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (16, N'кВт')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (17, N'кг')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (18, N'кг/м^3')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (19, N'кг/ч')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (20, N'ккал/(кг*Град.С)')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (21, N'ккал/(м^3*ч*Град.С)')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (22, N'ккал/(ч*м)')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (23, N'ккал/(ч*м*Град.С)')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (24, N'ккал/(ч*м^2*Град.С)')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (25, N'ккал/Град.С')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (26, N'ккал/ч')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (27, N'л/ч')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (28, N'м')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (29, N'м*ч^2/кг^2')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (30, N'м*ч^2/т^2')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (31, N'м.вод.ст')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (32, N'м/c')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (33, N'м/м')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (34, N'м^2')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (35, N'м^2/с')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (36, N'м^3')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (37, N'м^3*ч/Гкал')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (38, N'м^3/МВт')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (39, N'м^3/ч')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (40, N'мА')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (41, N'Мвт')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (42, N'мм')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (43, N'Мпа')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (44, N'МПа*ч^2/кг^2')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (45, N'МПа*ч^2/т^2')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (46, N'об/мин')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (47, N'Па/м')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (48, N'раз(а)')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (49, N'сут')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (50, N'т/Гкал')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (51, N'т/м^3')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (52, N'т/ч')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (53, N'час')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (54, N'чел')
INSERT [dbo].[edinitsy_aleksey] ([id], [stand_razmer]) VALUES (55, N'штук(и)')
SET IDENTITY_INSERT [dbo].[edinitsy_aleksey] OFF
SET IDENTITY_INSERT [dbo].[edinitsy_izmereniya] ON 

INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (1, 1, N'м*ч^2/т^2', 1, N'Гидравлическое сопротивление')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (2, 2, N'м*ч^2/кг^2', 1000000, N'Гидравлическое сопротивление')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (3, 3, N'МПа*ч^2/т^2', 0.00981, N'Гидравлическое сопротивление')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (4, 4, N'МПа*ч^2/кг^2', 9810, N'Гидравлическое сопротивление')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (5, 5, N'Гкал', 1, N'Количество теплоты')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (6, 6, N'ГДж', 0.239, N'Количество теплоты')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (7, 7, N'м.вод.ст', 1, N'Напор')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (8, 8, N'Мпа', 0.01, N'Напор')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (9, 9, N'бар', 0.1, N'Напор')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (10, 10, N'т/ч', 1, N'Расход')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (11, 11, N'кг/ч', 0.001, N'Расход')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (12, 12, N'м^3/ч', 1, N'Расход')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (13, 13, N'л/ч', 0.001, N'Расход')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (14, 14, N'Гкал/ч', 1, N'Тепловая мощность')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (15, 15, N'Мвт', 0.86, N'Тепловая мощность')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (16, 16, N'ккал/(кг*Град.С)', 1, N'Удельная теплоемкость')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (17, 17, N'Дж/(кг*К)', 4.187, N'Удельная теплоемкость')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (18, 18, N'ккал/Град.С', 1, N'Теплоемкость системы')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (19, 19, N'Дж/К', 4.187, N'Теплоемкость системы')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (20, 20, N'ккал/(ч*м^2*Град.С)', 1, N'Коэффициент теплоотдачи(теплопередачи)')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (21, 21, N'Вт/(м^2*К)', 0.86, N'Коэффициент теплоотдачи(теплопередачи)')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (22, 22, N'ккал/(ч*м*Град.С)', 1, N'Коэффициент теплопроводности')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (23, 23, N'Вт/(м*К)', 0.86, N'Коэффициент теплопроводности')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (24, 24, N'ккал/(ч*м)', 1, N'Нормы тепловых потерь(плотности теплового потока)')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (25, 25, N'Вт/м', 0.86, N'Нормы тепловых потерь(плотности теплового потока)')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (26, 26, N'т/м^3', 1, N'Плотность')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (27, 27, N'кг/м^3', 0.001, N'Плотность')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (28, 28, N'м^3*ч/Гкал', 1, N'Удельный объем воды')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (29, 29, N'м^3/МВт', 0.86, N'Удельный объем воды')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (30, 30, N'т/Гкал', 1, N'Удельный расход, тепло/вода')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (31, 31, NULL, 0, N'Удельный расход, тепло/вода')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (32, 32, NULL, 1, N'Удельный расход, вода/тепло')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (33, 33, NULL, 0, N'Удельный расход, вода/тепло')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (34, 34, N'м.вод.ст.', 1, N'Давление')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (35, 35, N'кгс/см^2', 10, N'Давление')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (36, 36, N'ккал/ч', 1, N'Тепловая производительнеость')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (37, 37, N'кВт', 860, N'Тепловая производительнеость')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (38, 38, N'м/м', 1, N'Удельные потери давления')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (39, 39, N'Па/м', 0.000102, N'Удельные потери давления')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (40, 40, N'ккал/(м^3*ч*Град.С)', 1, N'Удельные тепловые характеристики')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (41, 41, N'Вт/(м^3*К)', 0.86, N'Удельные тепловые характеристики')
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (42, 42, NULL, 1, NULL)
INSERT [dbo].[edinitsy_izmereniya] ([id], [kodvelichini], [razmer], [k_razmer], [velichina]) VALUES (43, 43, NULL, NULL, NULL)
SET IDENTITY_INSERT [dbo].[edinitsy_izmereniya] OFF
SET IDENTITY_INSERT [dbo].[edinitsypolzovatelya] ON 

INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (1, N'Вязкость', 0, N'м^2/с')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (2, N'Гидравлическое сопротивление', 1, N'м*ч^2/т^2')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (3, N'Давление', 34, N'м')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (4, N'Количество теплоты', 5, N'Гкал')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (5, N'Коэффициент теплоотдачи(теплопередачи)', 20, N'ккал/(ч*м^2*Град.С)')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (6, N'Коэффициент теплопроводности', 22, N'ккал/(ч*м*Град.С)')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (7, N'Напор', 7, N'м.вод.ст')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (8, N'Нормы тепловых потерь(плотности теплового потока)', 25, N'ккал/(ч*м)')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (9, N'Плотность', 26, N'т/м^3')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (10, N'Расход', 10, N'т/ч')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (11, N'Тепловая мощность', 14, N'Гкал/ч')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (12, N'Тепловая производительность', 36, N'ккал/ч')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (13, N'Теплоемкость системы', 18, N'ккал/Град.С')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (14, N'Удельная теплоемкость', 16, N'ккал/(кг*Град.С)')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (15, N'Удельные потери давления', 0, N'м/м')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (16, N'Удельные тепловые характеристики', 40, N'ккал/(м^3*ч*Град.С)')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (17, N'Удельный объем воды', 28, N'м^3*ч/Гкал')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (18, N'Удельный расход воды', 32, N'т/Гкал')
INSERT [dbo].[edinitsypolzovatelya] ([id], [velichina], [kodvelichini], [stand_razmer]) VALUES (19, N'Усилие', 0, NULL)
SET IDENTITY_INSERT [dbo].[edinitsypolzovatelya] OFF
